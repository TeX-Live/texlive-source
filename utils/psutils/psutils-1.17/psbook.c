/* psbook.c
 * Copyright (C) Angus J. C. Duggan 1991-1995
 * See file LICENSE for details.
 *
 * rearrange pages in conforming PS file for printing in signatures
 *
 * Usage:
 *       psbook [-q] [-s<signature>] [infile [outfile]]
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
   fprintf(stderr, "Usage: %s [-q] [-s<signature>] [infile [outfile]]\n",
	   program);
   fprintf(stderr, "       <signature> must be positive and divisible by 4\n");
   fflush(stderr);
   exit(1);
}


int main(int argc, char *argv[])
{
   int signature = 0;
   int currentpg, maxpage;

   infile = stdin;
   outfile = stdout;
   verbose = 1;
   for (program = *argv++; --argc; argv++) {
      if (argv[0][0] == '-') {
	 switch (argv[0][1]) {
	 case 's':	/* signature size */
	    signature = atoi(*argv+2);
	    if (signature < 1 || signature % 4) usage();
	    break;
	 case 'q':	/* quiet */
	    verbose = 0;
	    break;
	 case 'v':	/* version */
	 default:
	    usage();
	 }
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

   if (!signature)
      signature = maxpage = pages+(4-pages%4)%4;
   else
      maxpage = pages+(signature-pages%signature)%signature;

   /* rearrange pages */
   writeheader(maxpage);
   writeprolog();
   writesetup();
   for (currentpg = 0; currentpg < maxpage; currentpg++) {
      int actualpg = currentpg - currentpg%signature;
      switch(currentpg%4) {
      case 0:
      case 3:
	 actualpg += signature-1-(currentpg%signature)/2;
	 break;
      case 1:
      case 2:
	 actualpg += (currentpg%signature)/2;
	 break;
      }
      if (actualpg < pages)
	 writepage(actualpg);
      else
	 writeemptypage();
   }
   writetrailer();

   return 0;
}
