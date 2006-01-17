#include "dialogconfig.h"

/* GNU Library doesn't have toupper().  Until GNU gets this fixed, I will
   have to do it. */
#ifndef toupper
#define toupper(c) ((c) - 32)
#endif

#define coerce_to_upper(c) ((islower(c) ? toupper(c) : (c)))

#if !defined (HAVE_STRCASECMP)
int strcasecmp (char *string1, char * string2)
{
  char ch1, ch2;

  for (;;)
    {
      ch1 = *string1++;
      ch2 = *string2++;

      if (!(ch1 | ch2))
        return (0);

      ch1 = coerce_to_upper (ch1);
      ch2 = coerce_to_upper (ch2);

      if (ch1 != ch2)
        return (ch1 - ch2);
    }
}
#endif /* !HAVE_STRCASECMP */
