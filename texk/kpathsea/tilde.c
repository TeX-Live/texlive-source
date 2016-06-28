/* tilde.c: expand user's home directories.

    Copyright 1997, 1998, 2005, Olaf Weber.
    Copyright 1993, 1995, 1996, 1997, 2008, 2011, 2016 Karl Berry.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this library; if not, see <http://www.gnu.org/licenses/>.  */

#include <kpathsea/config.h>

#include <kpathsea/c-pathch.h>
#include <kpathsea/tilde.h>

#undef USE_GETPWNAM
#ifdef HAVE_PWD_H
#include <pwd.h>
#define USE_GETPWNAM 1
#elif defined (WIN32) && !defined (__MINGW32__)
#define USE_GETPWNAM 1
#endif

#ifdef WIN32
#define HOMEVAR "USERPROFILE"
#else
#define HOMEVAR "HOME"
#endif

/* If NAME has a leading ~ or ~user, Unix-style, expand it to the user's
   home directory, and return a new malloced string.  If no ~, or no
   <pwd.h>, just return NAME.  */

string
kpathsea_tilde_expand (kpathsea kpse, string name)
{
  string expansion;
  const_string home;
  const_string prefix;
#if defined(WIN32)
  string p;
#endif

  (void)kpse; /* currenty not used */
  assert (name);

  /* If there is a leading "!!", set prefix to "!!", otherwise use
     the empty string.  After this, we can test whether a prefix was
     found by checking *prefix, and it is safe to unconditionally
     prepend it. */
  if (name[0] == '!' && name[1] == '!') {
    name += 2;
    prefix = "!!";
  } else {
    prefix = "";
  }

  /* If no leading tilde, do nothing, and return the original string.  */
  if (*name != '~'
#ifndef USE_GETPWNAM
      /* Same for `~user' or `~user/', but no passwd database.  */
      || (name[1] && !IS_DIR_SEP (name[1]))
#endif
     ) {
    if (*prefix)
      name -= 2;
    expansion = name;

  } else {
    /* If a bare tilde, return the home directory or `.'; if just `~user',
       return that user's home directory or `.'.  Very unlikely that the
       directory name will do anyone any good, but ...  */
    unsigned c;

#ifdef USE_GETPWNAM
    /* If `~user' or `~user/', look up user in the passwd database.  */
    if (name[1] && !IS_DIR_SEP (name[1])) {
      struct passwd *p;
      string user;
      c = 2;
      while (!IS_DIR_SEP (name[c]) && name[c] != 0) {  /* find user name */
#if defined(WIN32)
        if (kpathsea_IS_KANJI(kpse, name+c))
          c++;
#endif
        c++;
      }

      user = (string) xmalloc (c);
      strncpy (user, name + 1, c - 1);
      user[c - 1] = 0;

#if defined(WIN32) && !defined(__MINGW32__)
      p = kpathsea_getpwnam (kpse, user);
#else
      /* We only need the cast here for (deficient) systems
         which do not declare `getpwnam' in <pwd.h>.  */
      p = (struct passwd *) getpwnam (user);
#endif
      free (user);

      /* If no such user, just use `.'.  */
      home = p ? p->pw_dir : ".";
    } else
#endif /* USE_GETPWNAM */
    {
      c = 1;
      home = getenv (HOMEVAR);
      if (!home)
        home = ".";
    }

    /* handle leading // */
    if (IS_DIR_SEP (*home) && IS_DIR_SEP (home[1]))
      home++;

    /* If HOME ends in /, omit the / in ~/ or ~user/.  */
    if (name[c]) {
#if defined(WIN32)
      const_string q;

      for (q = home; *q; q++) {
        if (IS_DIR_SEP (*q) && q[1] == 0)
          c++;
        else if (kpathsea_IS_KANJI(kpse, q))
          q++;
      }
#else
      if (IS_DIR_SEP (home[strlen (home) - 1]))
        c++; 
#endif
    }

    expansion = concat3 (prefix, home, name + c);
  }

#if defined(WIN32)
  for (p = expansion; *p; p++) {
    if (*p == '\\')
      *p = '/'; 
    else if (kpathsea_IS_KANJI(kpse, p))
      p++;
  }
#endif

  /* We may return the same thing as the original, and then we might not
     be returning a malloc-ed string.  Callers beware.  Sorry.  */
  return expansion;
}

#ifdef TEST

void
test_expand_tilde (const_string filename)
{
  string answer;

  printf ("Tilde expansion of `%s':\t", filename ? filename : "(nil)");
  answer = kpathsea_tilde_expand (kpse_def, (string)filename);
  puts (answer);
}

int
main (int argc, char **argv)
{
  string tilde_path = "tilde";
  kpse_set_program_name(argv[0],NULL);
  test_expand_tilde ("");
  test_expand_tilde ("none");
  test_expand_tilde ("~root");
  test_expand_tilde ("~");
  test_expand_tilde ("foo~bar");

  test_expand_tilde ("!!");
  test_expand_tilde ("!!none");
  test_expand_tilde ("!!~root");
  test_expand_tilde ("!!~");
  test_expand_tilde ("!!foo~bar");

  return 0;
}

#endif /* TEST */


/*
Local variables:
standalone-compile-command: "gcc -g -I. -I.. -DTEST tilde.c kpathsea.a"
End:
*/
