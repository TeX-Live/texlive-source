/* usage.c: Output a help message (from help.h).

   Modified in 2001 by O. Weber.
   Written in 1995 by K. Berry.  Public domain.  */

#include <w2c/config.h>

/* Print a message about trying --help with program name STR and exit
   with bad status. */

void
usage (const_string str)
{
  fprintf (stderr, "Try `%s --help' for more information.\n", str);
  uexit (1);
}


/* Print MESSAGE, a NULL-terminated array of strings which make up the
   help message, and exit successfully.  Each string is printed on a
   separate line. We use arrays instead of a single string to work
   around compiler limitations (sigh). Then print a line about sending
   bug reports to BUG_EMAIL, which defaults to tex-k@tug.org if passed
   NULL. If BUG_EMAIL is @tug.org, also output a url to the mailing list. */

void
usagehelp (const_string *message, const_string bug_email)
{
    if (!bug_email) {
      bug_email = "tex-k@tug.org";
    }
    
    while (*message) {
      printf ("%s\n", *message);
      ++message;
    }

    printf ("\nEmail bug reports to %s", bug_email);
    if (strlen (bug_email) > 9) {
      const_string domain = strchr (bug_email, '@');
      if (domain && strcmp (domain, "@tug.org") == 0) {
        const_string ptr;
        
        printf (" (https://lists.tug.org/");
        for (ptr = bug_email; ptr < domain; ptr++) {
          putchar (*ptr);
        }
        printf (")");
      }
    }
    puts (".");
    uexit (0);
}
