/*
 * A simple memset() in case your ANSI C does not provide it
 */

memset(void *s, int c, int length)
{  char *p = s;
  
   while (length--) *(p++) = c;
}
