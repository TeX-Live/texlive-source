Changes for the REFSORT utility from the CTWILL tarball.

This minimal set of changes tries to satisfy the GCC compiler.

This file is not copyrighted and can be used freely.

@x [0] l.2
\datethis
@y
\datethis
\let\maybe=\iffalse % print only sections that change
@z

@x [1] l.35 extend index entries for HiTeX and Inverse Adding-Doubling
@d max_key 30 /* greater than the length of the longest identifier */
@d max_size 100 /* greater than the length of the longest mini-index entry */
@y
@d max_key 50 /* greater than the length of the longest identifier */
@d max_size 120 /* greater than the length of the longest mini-index entry */
@z

@x [2] l.43 standard C library interface
#include "stdio.h"
#include "strings.h"
#include "ctype.h"
@y
#include <stdio.h>
#include <string.h>
#include <ctype.h>
@z

@x [2] l.57 declare return type
main()
@y
int main()
@z

@x [2] l.59 'register' removed in C++17.
  register char *p,*q;
  register int n; /* current number of items */
  register item *x, **y;
@y
  char *p,*q;
  int n; /* current number of items */
  item *x, **y;
@z

@x [5] l.89 variable not used
{ register int k;
@y
{
@z

@x [9] l.149 'register' removed in C++17
{ register int toggle=0;
@y
{ int toggle=0;
@z

@x [10] l.157 'register' removed in C++17
{ register int toggle=0;
@y
{ int toggle=0;
@z

@x [10] l.162 add 'Index.' section
  for (;*p;p++) *q++=*p;
}

@y
  for (;*p;p++) *q++=*p;
}

@* Index.
@z
