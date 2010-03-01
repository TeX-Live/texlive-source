/* FILE:    basics.c
 * PURPOSE: basic functions
 * AUTHOR:  Piet Tutelaers
 * VERSION: 1.0 (September 1995)
 */

#include "basics.h"

/* Give up ... */
void fatal(char *fmt, ...)
{  va_list args;

   va_start(args, fmt);
   vfprintf(stderr, fmt, args);
   va_end(args);
   exit(1);
}

/* Give a message ... */
void msg(char *fmt, ...)
{  va_list args;

   va_start(args, fmt);
   vfprintf(stderr, fmt, args);
   fflush(stderr);
   va_end(args);
}
