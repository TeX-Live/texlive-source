/*
 * A simple memset() in case your ANSI C does not provide it
 */

#if 0 /* not needed for xdvik */
memset(void *s, int c, int length)
{  char *p = s;
  
   while (length--) *(p++) = c;
}
#endif
