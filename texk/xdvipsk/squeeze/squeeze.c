/*   $Id: squeeze.c 14277 2009-07-16 12:00:58Z peter $
 *   Copyright 1986-2008 Tomas Rokicki.
 *   You may freely use, modify and/or distribute this program or any
 *   portion thereof.
 *   This routine squeezes a PostScript file down to its
 *   minimum.  We parse and then output it.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#ifndef O_BINARY
# ifdef _O_BINARY
#  define O_BINARY _O_BINARY
# else
#  define O_BINARY 0
# endif
#endif /* !O_BINARY */

#if O_BINARY
# ifdef __CYGWIN__
#  include <unistd.h>
#  include <io.h>
#  define SET_BINARY(fd) setmode((fd), O_BINARY)
# else
#  define SET_BINARY(fd) _setmode((fd), O_BINARY)
# endif
#endif

#define LINELENGTH (72)
#define BUFLENGTH (1000)

static char buf[BUFLENGTH] ;
static FILE *in, *out ;
static int linepos = 0 ;
static int lastspecial = 1 ;
/*
 *   This next routine writes out a `special' character.  In this case,
 *   we simply put it out, since any special character terminates the
 *   preceding token.
 */
static void
specialout (char c)
{
   if (linepos + 1 > LINELENGTH) {
      putchar('\n') ;
      linepos = 0 ;
   }
   putchar(c) ;
   linepos++ ;
   lastspecial = 1 ;
}

static void
strout (char *s)
{
   if (linepos + strlen(s) > LINELENGTH) {
      putchar('\n') ;
      linepos = 0 ;
   }
   linepos += strlen(s) ;
   while (*s != 0)
      putchar(*s++) ;
   lastspecial = 1 ;
}

static void
cmdout (char *s)
{
   int l ;

   l = strlen(s) ;
   if (linepos + l + 1 > LINELENGTH) {
      putchar('\n') ;
      linepos = 0 ;
      lastspecial = 1 ;
   }
   if (! lastspecial && *s != '/') {
      putchar(' ') ;
      linepos++ ;
   }
   while (*s != 0) {
      putchar(*s++) ;
   }
   linepos += l ;
   lastspecial = 0 ;
}


int
main (int argc, char *argv[])
{
   int c ;
   char *b ;
   char seeking ;

   if (argc > 3 || (in=(argc < 2 ? stdin : fopen(argv[1], "r")))==NULL ||
                    (out=(argc < 3 ? stdout : fopen(argv[2], "w")))==NULL) {
      (void)fprintf(stderr, "Usage:  squeeze [infile [outfile]]\n") ;
      exit(1) ;
   }
   /* Binary output is safer (for those systems which care to know the
      difference) since PostScript can include non-printable characters.  */
#if O_BINARY
   if (!isatty(fileno(in)))
      SET_BINARY(fileno(in)) ;
   if (!isatty(fileno(out)))
      SET_BINARY(fileno(out)) ;
#endif
   (void)fprintf(out, "%%!\n") ;
   while (1) {
      c = getc(in) ;
      if (c==EOF)
         break ;
      if (c=='%') {
         while ((c=getc(in))!='\n') ;
      }
      if (c <= ' ')
         continue ;
      switch (c) {
case '{' :
case '}' :
case '[' :
case ']' :
         specialout((char) c) ;
         break ;
case '<' :
case '(' :
         if (c=='(')
            seeking = ')' ;
         else
            seeking = '>' ;
         b = buf ;
         *b++ = c ;
         do {
            c = getc(in) ;
            if (b > buf + BUFLENGTH-2) {
               (void)fprintf(stderr, "Overran buffer seeking %c", seeking) ;
               exit(1) ;
            }
            *b++ = c ;
            if (c=='\\')
               *b++ = getc(in) ;
         } while (c != seeking) ;
         *b++ = 0 ;
         strout(buf) ;
         break ;
default:
         b = buf ;
         while ((c>='A'&&c<='Z')||(c>='a'&&c<='z')||
                (c>='0'&&c<='9')||(c=='/')||(c=='@')||
                (c=='!')||(c=='"')||(c=='&')||(c=='*')||(c==':')||
                (c==',')||(c==';')||(c=='?')||(c=='^')||(c=='~')||
                (c=='-')||(c=='.')||(c=='#')||(c=='|')||(c=='_')||
                (c=='=')||(c=='$')||(c=='+')) {
            *b++ = c ;
            c = getc(in) ;
         }
         if (b == buf) {
            (void)fprintf(stderr, "Oops!  Missed a case: %c.\n", c) ;
            exit(1) ;
         }
         *b++ = 0 ;
         (void)ungetc(c, in) ;
         cmdout(buf) ;
      }
   }
   if (linepos != 0)
      putchar('\n') ;
   exit(0) ;
   /*NOTREACHED*/
}
