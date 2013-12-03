/* psnup.c
 * Put multiple pages on to one page
 *
 * (c) Reuben Thomas 2012
 * (c) Angus J. C. Duggan 1991-1997
 * See file LICENSE for details.
 */

#include "config.h"
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <string.h>
#include <paper.h>

#include "psutil.h"
#include "psspec.h"

const char *syntax = "[-q] [-wWIDTH] [-hHEIGHT] [-pPAPER] [-WWIDTH] [-HHEIGHT] [-PPAPER] [-l] [-r] [-c] [-f] [-mMARGIN] [-bBORDER] [-dLWIDTH] [-sSCALE] [-NUP] [INFILE [OUTFILE]]\n";

const char *argerr_message = "bad dimension\n";

/* return next larger exact divisor of number, or 0 if none. There is probably
 * a much more efficient method of doing this, but the numbers involved are
 * small, so it's not a big loss. */
_GL_ATTRIBUTE_CONST static int nextdiv(int n, int m)
{
   while (++n <= m) {
      if (m%n == 0)
	 return (n);
   }
   return (0);
}

int
main(int argc, char *argv[])
{
   int horiz = 0, vert = 0, rotate = 0, column = 0;
   int flip = 0, leftright = 0, topbottom = 0;
   int nup = 1;
   double draw = 0;				/* draw page borders */
   double scale = 1.0;				/* page scale */
   double uscale = 0;				/* user supplied scale */
   double ppwid, pphgt;				/* paper dimensions */
   double margin, border;			/* paper & page margins */
   double vshift, hshift;			/* page centring shifts */
   double iwidth, iheight ;			/* input paper size */
   double tolerance = 100000;			/* layout tolerance */
   off_t sizeheaders[20];			/* headers to remove */
   int opt;
   const struct paper *paper = NULL;

   SET_BINARY(stdin);
   SET_BINARY(stdout);

   set_paper_size(NULL);

   margin = border = vshift = hshift = column = flip = 0;
   leftright = topbottom = 1;
   iwidth = iheight = -1 ;

   verbose = 1;
   program = *argv;

   while((opt =
          getopt(argc, argv,
                 "qd::lrfcw:W:h:H:m:b:t:s:p:P:n:1::2::3::4::5::6::7::8::9::"))
         != EOF) {
     switch(opt) {
     case 'q':	/* quiet */
       verbose = 0;
       break;
     case 'd':	/* draw borders */
       if (optarg)
         draw = singledimen(optarg);
       else
         draw = 1;
       break;
     case 'l':	/* landscape (rotated left) */
       column = !column;
       topbottom = !topbottom;
       break;
     case 'r':	/* seascape (rotated right) */
       column = !column;
       leftright = !leftright;
       break;
     case 'f':	/* flipped */
       flip = 1;
       break;
     case 'c':	/* column major layout */
       column = !column;
       break;
     case 'w':	/* page width */
       width = singledimen(optarg);
       break;
     case 'W':	/* input page width */
       iwidth = singledimen(optarg);
       break;
     case 'h':	/* page height */
       height = singledimen(optarg);
       break;
     case 'H':	/* input page height */
       iheight = singledimen(optarg);
       break;
     case 'm':	/* margins around whole page */
       margin = singledimen(optarg);
       break;
     case 'b':	/* border around individual pages */
       border = singledimen(optarg);
       break;
     case 't':	/* layout tolerance */
       tolerance = atof(optarg);
       break;
     case 's':	/* override scale */
       uscale = atof(optarg);
       break;
     case 'p':	/* output (and by default input) paper type */
       if ( (paper = paperinfo(optarg)) != NULL ) {
         width = paperpswidth(paper);
         height = paperpsheight(paper);
       } else
         message(FATAL, "paper size '%s' not recognised\n", optarg);
       break;
     case 'P':	/* paper type */
       if ( (paper = paperinfo(optarg)) != NULL ) {
         iwidth = paperpswidth(paper);
         iheight = paperpsheight(paper);
       } else
         message(FATAL, "paper size '%s' not recognised\n", optarg);
       break;
     case 'n':	/* n-up, for compatibility with other psnups */
       if ((nup = atoi(optarg)) < 1)
         message(FATAL, "-n %d too small\n", nup);
       break;
     case '1':
     case '2':
     case '3':
     case '4':
     case '5':
     case '6':
     case '7':
     case '8':
     case '9':
       if(optarg) {
         char *valuestr = (char *) malloc(strlen(optarg) + 2);
         valuestr[0] = opt;
         strcpy(&(valuestr[1]), optarg);

         /* really should check that valuestr is only digits here...*/
         if ((nup = atoi(valuestr)) < 1)
           message(FATAL, "-n %d too small\n", nup);
         free(valuestr);
       } else {
         nup = (opt - '0');
       }
       break;
     case 'v':	/* version */
     default:
       usage();
     }
   }

   infile = stdin;
   outfile = stdout;

   /* Be defensive */
   if((argc - optind) < 0 || (argc - optind) > 2) usage();

   if (optind != argc) {
     /* User specified an input file */
     if ((infile = fopen(argv[optind], "rb")) == NULL)
       message(FATAL, "can't open input file %s\n", argv[optind]);
     optind++;
   }

   if (optind != argc) {
     /* User specified an output file */
     if ((outfile = fopen(argv[optind], "wb")) == NULL)
       message(FATAL, "can't open output file %s\n", argv[optind]);
     optind++;
   }

   if (optind != argc) usage();

   if ((infile=seekable(infile))==NULL)
      message(FATAL, "can't seek input\n");

   if (width <= 0 || height <= 0)
      message(FATAL, "page width and height must be set\n");

   /* subtract paper margins from height & width */
   ppwid = width - margin*2;
   pphgt = height - margin*2;

   if (ppwid <= 0 || pphgt <= 0)
      message(FATAL, "paper margins are too large\n");

   scanpages(sizeheaders);

   /* set default values of input height & width */
   if ( iwidth > 0 && width == -1 )
     width = iwidth ;
   if ( iheight > 0 && height == -1 )
     height = iheight ;

   /* Finding the best layout is an optimisation problem. We try all of the
    * combinations of width*height in both normal and rotated form, and
    * minimise the wasted space. */
   {
      double best = tolerance;
      int hor;
      for (hor = 1; hor; hor = nextdiv(hor, nup)) {
	 int ver = nup/hor;
	 /* try normal orientation first */
	 double scl = MIN(pphgt/(height*ver), ppwid/(width*hor));
	 double optim = (ppwid-scl*width*hor)*(ppwid-scl*width*hor) +
	    (pphgt-scl*height*ver)*(pphgt-scl*height*ver);
	 if (optim < best) {
	    best = optim;
	    /* recalculate scale to allow for internal borders */
	    scale = MIN((pphgt-2*border*ver)/(height*ver),
			(ppwid-2*border*hor)/(width*hor));
	    hshift = (ppwid/hor - width*scale)/2;
	    vshift = (pphgt/ver - height*scale)/2;
	    horiz = hor; vert = ver;
	    rotate = flip;
	 }
	 /* try rotated orientation */
	 scl = MIN(pphgt/(width*hor), ppwid/(height*ver));
	 optim = (pphgt-scl*width*hor)*(pphgt-scl*width*hor) +
	    (ppwid-scl*height*ver)*(ppwid-scl*height*ver);
	 if (optim < best) {
	    best = optim;
	    /* recalculate scale to allow for internal borders */
	    scale = MIN((pphgt-2*border*hor)/(width*hor),
			(ppwid-2*border*ver)/(height*ver));
	    hshift = (ppwid/ver - height*scale)/2;
	    vshift = (pphgt/hor - width*scale)/2;
	    horiz = ver; vert = hor;
	    rotate = !flip;
	 }
      }

      /* fail if nothing better than worst tolerance was found */
      if (best == tolerance)
	 message(FATAL, "can't find acceptable layout for %d-up\n", nup);
   }

   if (flip) {	/* swap width & height for clipping */
      double tmp = width;
      width = height;
      height = tmp;
   }

   if (rotate) {	/* rotate leftright and topbottom orders */
      int tmp = topbottom;
      topbottom = !leftright;
      leftright = tmp;
      column = !column;
   }

   /* now construct specification list and run page rearrangement procedure */
   {
      int page = 0;
      PageSpec *specs, *tail;

      tail = specs = newspec();

      while (page < nup) {
	 int up, across;		/* page index */

	 if (column) {
	    if (leftright)		/* left to right */
	       across = page/vert;
	    else			/* right to left */
	       across = horiz-1-page/vert;
	    if (topbottom)		/* top to bottom */
	       up = vert-1-page%vert;
	    else			/* bottom to top */
	       up = page%vert;
	 } else {
	    if (leftright)		/* left to right */
	       across = page%horiz;
	    else			/* right to left */
	       across = horiz-1-page%horiz;
	    if (topbottom)		/* top to bottom */
	       up = vert-1-page/horiz;
	    else			/* bottom to top */
	       up = page/horiz;
	 }
	 if (rotate) {
	    tail->xoff = margin + (across+1)*ppwid/horiz - hshift;
	    tail->rotate = 90;
	    tail->flags |= ROTATE;
	 } else {
	    tail->xoff = margin + across*ppwid/horiz + hshift;
	 }
	 tail->pageno = page;
	 if (uscale > 0)
	    tail->scale = uscale;
	 else
	    tail->scale = scale;
	 tail->flags |= SCALE;
	 tail->yoff = margin + up*pphgt/vert + vshift;
	 tail->flags |= OFFSET;
	 if (++page < nup) {
	    tail->flags |= ADD_NEXT;
	    tail->next = newspec();
	    tail = tail->next;
	 }
      }
      
      pstops_write(nup, 1, 0, specs, draw, sizeheaders); /* do page rearrangement */
   }

   return 0;
}

