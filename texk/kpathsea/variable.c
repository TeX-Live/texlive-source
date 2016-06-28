/* variable.c: variable expansion.

    Copyright 1993, 1994, 1995, 1996, 2008, 2009, 2011, 2012, 2016 Karl Berry.
    Copyright 1997, 1999, 2001, 2002, 2005 Olaf Weber.

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

#include <kpathsea/c-ctype.h>
#include <kpathsea/cnf.h>
#include <kpathsea/fn.h>
#include <kpathsea/expand.h>
#include <kpathsea/variable.h>


/* Here's the simple one, when a program just wants a value.  */

string
kpathsea_var_value (kpathsea kpse, const_string var)
{
  string vtry, ret;
  const_string value;

  assert (kpse->program_name);

  /* First look for VAR.progname. */
  vtry = concat3 (var, ".", kpse->program_name);
  value = getenv (vtry);
  free (vtry);

  if (!value || !*value) {
    /* Now look for VAR_progname. */
    vtry = concat3 (var, "_", kpse->program_name);
    value = getenv (vtry);
    free (vtry);
  }

  /* Just plain VAR.  */
  if (!value || !*value)
    value = getenv (var);

  /* Not in the environment; check a config file.  */
  if (!value || !*value)
      value = kpathsea_cnf_get (kpse, var);

  /* We have a value; do variable and tilde expansion.  We want to use ~
     in the cnf files, to adapt nicely to Windows and to avoid extra /'s
     (see tilde.c), but we also want kpsewhich -var-value=foo to not
     have any literal ~ characters, so our shell scripts don't have to
     worry about doing the ~ expansion.  */
  ret = value ? kpathsea_expand (kpse, value) : NULL;

#ifdef KPSE_DEBUG
  if (KPATHSEA_DEBUG_P (KPSE_DEBUG_VARS))
    DEBUGF2("variable: %s = %s\n", var, ret ? ret : "(nil)");
#endif

  return ret;
}

#if defined (KPSE_COMPAT_API)
string
kpse_var_value (const_string var)
{
    return kpathsea_var_value (kpse_def,var);
}
#endif


/* We have to keep track of variables being expanded, otherwise
   constructs like TEXINPUTS = $TEXINPUTS result in an infinite loop.
   (Or indirectly recursive variables, etc.)  Our simple solution is to
   add to a list each time an expansion is started, and check the list
   before expanding.  */

static void
expanding (kpathsea kpse, const_string var, boolean xp)
{
  unsigned e;
  for (e = 0; e < kpse->expansion_len; e++) {
    if (STREQ (kpse->expansions[e].var, var)) {
      kpse->expansions[e].expanding = xp;
      return;
    }
  }

  /* New variable, add it to the list.  */
  kpse->expansion_len++;
  XRETALLOC (kpse->expansions, kpse->expansion_len, expansion_type);
  kpse->expansions[kpse->expansion_len - 1].var = xstrdup (var);
  kpse->expansions[kpse->expansion_len - 1].expanding = xp;
}


/* Return whether VAR is currently being expanding.  */

static boolean
expanding_p (kpathsea kpse, const_string var)
{
  unsigned e;
  for (e = 0; e < kpse->expansion_len; e++) {
    if (STREQ (kpse->expansions[e].var, var))
      return kpse->expansions[e].expanding;
  }

  return false;
}

/* Append the result of value of `var' to EXPANSION, where `var' begins
   at START and ends at END.  If `var' is not set, do not complain.
   Return 1 if `var' was defined, 0 if not.  This is a subroutine for
   the `kpathsea_var_expand' function.  */

static boolean
expand (kpathsea kpse, fn_type *expansion,
        const_string start, const_string end)
{
  boolean ret = false;
  const_string value;
  unsigned len = end - start + 1;
  string var = (string)xmalloc (len + 1);
  strncpy (var, start, len);
  var[len] = 0;

  if (expanding_p (kpse, var)) {
    WARNING1 ("kpathsea: variable `%s' references itself (eventually)", var);
  } else {
    string vtry = concat3 (var, "_", kpse->program_name);
    /* Check for an environment variable.  */
    value = getenv (vtry);
    free (vtry);

    if (!value || !*value)
      value = getenv (var);

    /* If no envvar, check the config files.  */
    if (!value || !*value)
      value = kpathsea_cnf_get (kpse, var);

    if (value) {
      string tmp;
      ret = true;
      expanding (kpse, var, true);
      tmp = kpathsea_expand (kpse, value);
      expanding (kpse, var, false);

      fn_grow (expansion, tmp, strlen (tmp));
      free (tmp);
    }
  }

  free (var);
  return ret;
}

/* Can't think of when it would be useful to change these (and the
   diagnostic messages assume them), but ... */
#ifndef IS_VAR_START /* starts all variable references */
#define IS_VAR_START(c) ((c) == '$')
#endif
#ifndef IS_VAR_CHAR  /* variable name constituent */
#define IS_VAR_CHAR(c) (ISALNUM (c) || (c) == '_')
#endif
#ifndef IS_VAR_BEGIN_DELIMITER /* start delimited variable name (after $) */
#define IS_VAR_BEGIN_DELIMITER(c) ((c) == '{')
#endif
#ifndef IS_VAR_END_DELIMITER
#define IS_VAR_END_DELIMITER(c) ((c) == '}')
#endif


/* Maybe we should support some or all of the various shell ${...}
   constructs, especially ${var-value}.  We do do ~ expansion.  */

string
kpathsea_var_expand (kpathsea kpse, const_string src)
{
  const_string s;
  string ret;
  fn_type expansion;
  expansion = fn_init ();

  /* Copy everything but variable constructs.  */
  for (s = src; *s; s++) {
    if (IS_VAR_START (*s)) {
      s++;

      /* Three cases: `$VAR', `${VAR}', `$<anything-else>'.  */
      if (IS_VAR_CHAR (*s)) {
        /* $V: collect name constituents, then expand.  */
        const_string var_end = s;

        do {
          var_end++;
        } while (IS_VAR_CHAR (*var_end));

        var_end--; /* had to go one past */
        if (!expand (kpse, &expansion, s, var_end)) {
          /* If no expansion, include the literal $x construct,
             so filenames containing dollar signs can be read.
             The first +1 is to get the full variable name,
             the other +1 is to get the dollar sign; we've moved past it.  */
          fn_grow (&expansion, s - 1, var_end - s + 1 + 1);
        }
        s = var_end;

      } else if (IS_VAR_BEGIN_DELIMITER (*s)) {
        /* ${: scan ahead for matching delimiter, then expand.  */
        const_string var_end = ++s;

        while (*var_end && !IS_VAR_END_DELIMITER (*var_end)) {
#if defined(WIN32)
          if (kpathsea_IS_KANJI(kpse, var_end))
            var_end++;
#endif
          var_end++;
        }

        if (! *var_end) {
          WARNING1 ("kpathsea: %s: No matching } for ${", src);
          s = var_end - 1; /* will incr to null at top of loop */
        } else {
          expand (kpse, &expansion, s, var_end - 1);
          s = var_end; /* will incr past } at top of loop*/
        }

      } else {
        /* $<something-else>: warn, but preserve characters; again, so
           filenames containing dollar signs can be read.  */
        WARNING2 ("kpathsea: %s: Unrecognized variable construct `$%c'",
                  src, *s);
        fn_grow (&expansion, s - 1, 2);  /* moved past the $  */
      }
    } else
     fn_1grow (&expansion, *s);
  }
  fn_1grow (&expansion, 0);

  ret = FN_STRING (expansion);
  return ret;
}

#if defined (KPSE_COMPAT_API)
string
kpse_var_expand (const_string src)
{
    return kpathsea_var_expand (kpse_def,src);
}
#endif


#ifdef TEST

static void
test_var (string test, string right_answer)
{
  string result = kpse_var_expand (test);

  printf ("expansion of `%s'\t=> %s", test, result);
  if (!STREQ (result, right_answer))
    printf (" [should be `%s']", right_answer);
  putchar ('\n');
}


int
main (int argc, char **argv)
{
  kpse_set_program_name(argv[0], NULL);
  test_var ("a", "a");
  test_var ("$foo", "");
  test_var ("a$foo", "a");
  test_var ("$foo a", " a");
  test_var ("a$foo b", "a b");

  xputenv ("FOO", "foo value");
  test_var ("a$FOO", "afoo value");

  xputenv ("Dollar", "$");
  test_var ("$Dollar a", "$ a");

  test_var ("a${FOO}b", "afoo valueb");
  test_var ("a${}b", "ab");

  test_var ("$$", ""); /* and error */
  test_var ("a${oops", "a"); /* and error */

  return 0;
}

#endif /* TEST */


/*
Local variables:
standalone-compile-command: "gcc -g -I. -I.. -DTEST variable.c kpathsea.a"
End:
*/
