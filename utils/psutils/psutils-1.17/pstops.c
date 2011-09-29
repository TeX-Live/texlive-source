/* pstops.c
 * Copyright (C) Angus J. C. Duggan 1991-1995
 * See file LICENSE for details.
 *
 * rearrange pages in conforming PS file for printing in signatures
 *
 * Usage:
 *       pstops [-q] [-b] [-d] [-w<dim>] [-h<dim>] [-ppaper] <pagespecs> [infile [outfile]]
 */

#include "psutil.h"
#include "psspec.h"
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
   fprintf(stderr, "Usage: %s [-q] [-b] [-wwidth] [-hheight] [-dlwidth] [-ppaper] <pagespecs> [infile [outfile]]\n",
	   program);
   fflush(stderr);
   exit(1);
}

static void argerror(void)
{
   fprintf(stderr, "%s: page specification error:\n", program);
   fprintf(stderr, "  <pagespecs> = [modulo:]<spec>\n");
   fprintf(stderr, "  <spec>      = [-]pageno[@scale][L|R|U][(xoff,yoff)][,spec|+spec]\n");
   fprintf(stderr, "                modulo>=1, 0<=pageno<modulo\n");
   fflush(stderr);
   exit(1);
}

static int modulo = 1;
static int pagesperspec = 1;

static PageSpec *parsespecs(char *str)
{
   PageSpec *head, *tail;
   int other = 0;
   int num = -1;

   head = tail = newspec();
   while (*str) {
      if (isdigit(*str)) {
	 num = parseint(&str, argerror);
      } else {
	 switch (*str++) {
	 case ':':
	    if (other || head != tail || num < 1) argerror();
	    modulo = num;
	    num = -1;
	    break;
	 case '-':
	    tail->reversed = !tail->reversed;
	    break;
	 case '@':
	    if (num < 0) argerror();
	    tail->scale *= parsedouble(&str, argerror);
	    tail->flags |= SCALE;
	    break;
	 case 'l': case 'L':
	    tail->rotate += 90;
	    tail->flags |= ROTATE;
	    break;
	 case 'r': case 'R':
	    tail->rotate -= 90;
	    tail->flags |= ROTATE;
	    break;
	 case 'u': case 'U':
	    tail->rotate += 180;
	    tail->flags |= ROTATE;
	    break;
	 case '(':
	    tail->xoff += parsedimen(&str, argerror);
	    if (*str++ != ',') argerror();
	    tail->yoff += parsedimen(&str, argerror);
	    if (*str++ != ')') argerror();
	    tail->flags |= OFFSET;
	    break;
	 case '+':
	    tail->flags |= ADD_NEXT;
	 case ',':
	    if (num < 0 || num >= modulo) argerror();
	    if ((tail->flags & ADD_NEXT) == 0)
	       pagesperspec++;
	    tail->pageno = num;
	    tail->next = newspec();
	    tail = tail->next;
	    num = -1;
	    break;
	 default:
	    argerror();
	 }
	 other = 1;
      }
   }
   if (num >= modulo)
      argerror();
   else if (num >= 0)
      tail->pageno = num;
   return (head);
}

int main(int argc, char *argv[])
{
   PageSpec *specs = NULL;
   int nobinding = 0;
   double draw = 0;
   Paper *paper;

#ifdef PAPER
   if ( (paper = findpaper(PAPER)) != (Paper *)0 ) {
      width = (double)PaperWidth(paper);
      height = (double)PaperHeight(paper);
   }
#endif

   infile = stdin;
   outfile = stdout;
   verbose = 1;
   for (program = *argv++; --argc; argv++) {
      if (argv[0][0] == '-') {
	 switch (argv[0][1]) {
	 case 'q':	/* quiet */
	    verbose = 0;
	    break;
	 case 'd':	/* draw borders */
	    if (argv[0][2])
	       draw = singledimen(*argv+2, argerror, usage);
	    else
	       draw = 1;
	    break;
	 case 'b':	/* no bind operator */
	    nobinding = 1;
	    break;
	 case 'w':	/* page width */
	    width = singledimen(*argv+2, argerror, usage);
	    break;
	 case 'h':	/* page height */
	    height = singledimen(*argv+2, argerror, usage);
	    break;
	 case 'p':	/* paper type */
	    if ( (paper = findpaper(*argv+2)) != (Paper *)0 ) {
	       width = (double)PaperWidth(paper);
	       height = (double)PaperHeight(paper);
	    } else
	      message(FATAL, "paper size '%s' not recognised\n", *argv+2);
	    break;
	 case 'v':	/* version */
	    usage();
	 default:
	    if (specs == NULL)
	       specs = parsespecs(*argv);
	    else
	       usage();
	 }
      } else if (specs == NULL)
	 specs = parsespecs(*argv);
      else if (infile == stdin) {
	 if ((infile = fopen(*argv, OPEN_READ)) == NULL)
	    message(FATAL, "can't open input file %s\n", *argv);
      } else if (outfile == stdout) {
	 if ((outfile = fopen(*argv, OPEN_WRITE)) == NULL)
	    message(FATAL, "can't open output file %s\n", *argv);
      } else usage();
   }
   if (specs == NULL)
      usage();
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

   pstops(modulo, pagesperspec, nobinding, specs, draw);

   return 0;
}
