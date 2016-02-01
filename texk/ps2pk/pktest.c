/* FILE:     pktest.c 
 * PURPOSE:  This program demonstrates how a PK file can be created from
 *           a single character bitmap.
 * USAGE:    pktest -c<char_code> -W<with> -H<Height> pkname < test.bm
 *	     (test.bm contains the character from `The GFtoPK processor')
 * AUTHOR:   Piet Tutelaers
 * (see README for license)
 */

int testing = 1;
#include <stdio.h>
#include "basics.h"	/* fatal() */
#include "pkout.h"

static int next_pixel(void);

int main(int argc, char *argv[])
{
   int done, C = 0, W = 0, H = 0, c;
   const char *myname;
   char *pkname, comment[256];
	
   myname = argv[0];
   while (--argc > 0 && (*++argv)[0] == '-') {
      done=0;
      while ((!done) && (c = *++argv[0])) /* allow multiletter options */
         switch (c) {
      	 case 'c':
      	    C = *++argv[0];
      	    if (C == '\0') {
      	       argc--;  C = *++argv[0];
      	    }
      	    break;
      	 case 'H':
      	    if (*++argv[0] == '\0') {
      	       argc--;  argv++;
      	    }
	    H = atoi(*argv); done = 1;
      	    break;
      	 case 'W':
      	    if (*++argv[0] == '\0') {
      	       argc--;  argv++;
      	    }
	    W = atoi(*argv); done = 1;
      	    break;
      	 default:
      	    fatal("%s: %c invalid option\n", myname, c);
      	 }
   }

   if (argc == 0 || C == 0 || W*H == 0) {
      msg  ("pktest (ps2pk) version " PACKAGE_VERSION "\n");
      fatal("Usage: %s -c<char> -W<width> -H<height> pkfile\n", myname);
   }

   pkname = argv[0];
   pk_open(pkname);
   
   sprintf(comment, "Testfont %s designed at 10 points", pkname);
   pk_preamble(comment, 10.0, 1473505522, 120, 120);
   printf("character %c Width %d Height %d\n", C, W, H);
   pk_char(C, 640796, 25, W, H, -2, 28, next_pixel);
   pk_postamble();
   pk_close();
   return 0;
}

/* This function delivers the pixels from the character's bounding box
 * from left to right and from top to bottom.
 */
static int next_pixel(void)
{  int c;
   do { c = getchar();
      if (c==EOF) fatal("reading past end of file!\n");
      if (c == '*' || c == 'X') return BLACK;
      if (c == '.') return WHITE;
   } while (1);
}

/* The character example from GFtoPK:
  ********************
  ********************
  ********************
  ********************
  **................**
  **................**
  **................**
  ....................
  ....................
  ..**............**..
  ..**............**..
  ..**............**..
  ..****************..
  ..****************..
  ..****************..
  ..****************..
  ..**............**..
  ..**............**..
  ..**............**..
  ....................
  ....................
  ....................
  **................**
  **................**
  **................**
  ********************
  ********************
  ********************
  ********************
*/
