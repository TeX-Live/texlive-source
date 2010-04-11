/* printversion.c: Output for the standard GNU option --version.

   Written in 1996 by Karl Berry.  Public domain.  */

#include "config.h"
#include "../../lib/lib.h"
#include "../kanji.h"
#include <kpathsea/version.h>

/* We're passed in the original WEB banner string, which has the form
This is PROGRAM, Version VERSION-NUMBER
   We parse the PROGRAM and VERSION-NUMBER out of this.
   
   If COPYRIGHT_HOLDER is specified and AUTHOR isn't, then use the
   former for the latter.  If AUTHOR is specified and COPYRIGHT_HOLDER
   isn't, it means the original program is public domain.
   
   Maybe I should have just done it all inline in each individual
   program, but tangle doesn't allow multiline string constants ...  */

void
printversionandexit (const_string banner,
                     const_string copyright_holder,  
                     const_string author,
                     char *extra_info)
{
  string prog_name;
  unsigned len;
  const_string prog_name_end = strchr (banner, ',');
  const_string prog_version = strrchr (banner, ' ');
  assert (prog_name_end && prog_version);
  prog_version++;
  
  /* attention: strlen(s)+1 = sizeof(s) */
  len = prog_name_end - banner - sizeof ("This is");
  prog_name = (string)xmalloc (len + 1);
  strncpy (prog_name, banner + sizeof ("This is"), len);
  prog_name[len] = 0;

  /* The Web2c version string starts with a space.  */
  printf ("%s %s (%s)%s\n", prog_name, prog_version, get_enc_string(),
          versionstring);
  puts (kpathsea_version_string);
  puts (ptexenc_version_string);

  if (copyright_holder) {
    printf ("Copyright 2009 %s.\n", copyright_holder);
    if (!author)
      author = copyright_holder;
  }

  puts ("There is NO warranty.  Redistribution of this software is");
  fputs ("covered by the terms of ", stdout);
  printf ("both the %s copyright and\n", prog_name);
  puts ("the Lesser GNU General Public License.");
  puts ("For more information about these matters, see the file");
  printf ("named COPYING and the %s source.\n", prog_name);
  printf ("Primary author of %s: %s.\n", prog_name, author);

  if (extra_info) {
    puts (extra_info);
  }

  free (prog_name); /* lost in lib/printversion.c */
  uexit (0);
}
