/* fixwrites -- convert Pascal write/writeln's into fprintf's or putc's.
   Originally by Tim Morgan, October 10, 1987.  */

#include <w2c/config.h>
#include <kpathsea/c-pathmx.h>

char buf[BUFSIZ], filename[PATH_MAX], args[100];
char *file, *argp, *as, *cmd;

int tex = false;

/* Replace the last (should be only) newline in S with a null.  */

static void
remove_newline (string s)
{
  char *temp = strrchr (s, '\n');
  if (temp == NULL)
    {
      fprintf (stderr, "Lost newline somehow.\n");
      /* This is so the convert script can delete the output file on error.  */
      puts ("@error@");
      exit (1);
    }

  *temp = 0;
}


static char *
insert_long (string cp)
{
  char tbuf[BUFSIZ];
  register int i;

  for (i = 0; &buf[i] < cp; ++i)
    tbuf[i] = buf[i];
  strcpy (&tbuf[i], "(long)");
  strcpy (&tbuf[i + 6], cp);
  strcpy (buf, tbuf);
  return cp + 6;
}


static void
join (string cp)
{
  char temp[BUFSIZ], *tp;

  if (!fgets (temp, BUFSIZ, stdin))
    return;
  remove_newline (temp);

  *cp++ = ' ';
  for (tp = temp; *tp == ' '; ++tp)
    ;

  strcpy (cp, tp);
}


static void
do_blanks (int indent)
{
  register int i;

  for (i = 0; i < indent / 8; i++)
    putchar ('\t');
  indent %= 8;
  for (i = 0; i < indent; i++)
    putchar (' ');
}


/* Return true if we have a whole write/writeln statement.  We determine
   this by matching parens, ignoring those within strings.  */
static int
whole (string cp)
{
  register int depth = 0;

  while (cp && *cp)
    {
      switch (*cp)
	{
	case '(':
	  ++depth;
	  break;
	case ')':
	  --depth;
	  break;
	case '"':
	  for (++cp; cp && *cp && *cp != '"'; ++cp)
	    if (*cp == '\\')
	      ++cp;
	  break;
	case '\'':
	  ++cp;
	  if (*cp == '\\') ++cp;
	  ++cp;
	  break;
	}
      ++cp;
    }
  return depth <= 0;
}


/* Skips to the next , or ), skipping over balanced paren pairs.  */

static char *
skip_balanced (string cp)
{
  register int depth = 0;

  while (depth > 0 || (*cp != ',' && *cp != ')'))
    {
      switch (*cp)
	{
	case '(':
	  ++depth;
	  break;
	case ')':
	  --depth;
	  break;
	}
      ++cp;
    }
  return cp;
}


/* Return true if c appears, except inside a quoted string */

static int
bare (string cp,  char c)
{
  for (; *cp && *cp != c; ++cp)
    {
      if (*cp == '"')
	{
	  ++cp;			/* skip over initial quotation mark */
	  while (*cp && *cp != '"')
	    {			/* skip to closing double quote */
	      if (*cp == '\\')
		++cp;
	      ++cp;
	    }
	}
      else if (*cp == '\'')
	{
	  ++cp;			/* skip to contained char */
	  if (*cp == '\'')
	    ++cp;		/* if backslashed, it's double */
	  ++cp;			/* skip to closing single-quote mark */
	}
    }
  return *cp;
}


/* xchr[...] is supposed to be replaced by Xchr(...)  when characters
   take more than a single octet each, as is the case in Aleph.  Now
   there are several occurrences of xchr[...[...]...], which are
   translated into Xchr(...[...)...], and the compiler dies on syntax
   errors.  Ensures that it is the matching bracket that is replaced,
   not the first one.  */

static char *
advance_cp (char *cp, int lefts)
{
  char *cp1;
  char *cp2;

  cp1 = strchr (cp + 1, ']');
  cp2 = strchr (cp + 1, '[');
  if (cp2 && cp2 < cp1)
    return advance_cp (cp2, lefts + 1);
  if (lefts == 1)
    return cp1;
  return advance_cp (cp1, lefts - 1);
}

#ifdef WIN32
#include <io.h>
#include <fcntl.h>
#endif

int
main (int argc,  string *argv)
{
  register char *cp;
  int blanks_done, indent, i;
  const char *program_name = "";

#ifdef WIN32
  setmode(fileno(stdout), _O_BINARY);
#endif
  for (i = 1; i < argc; i++)
    {
      if (STREQ(argv[i],"-t"))
	tex = true;
      else
	program_name = argv[i];
    }

  while (fgets (buf, BUFSIZ, stdin))
    {
      remove_newline (buf);
      blanks_done = false;

      for (cp = buf; *cp; ++cp) ;

      while (cp != buf && *--cp == ' ') ;

      while (*cp == '.')
	{
	  join (cp + 1);
	  while (*cp)
	    ++cp;
	  while (*--cp == ' ') ;
	}

      for (cp = buf, indent = 0; *cp == ' ' || *cp == '\t'; ++cp)
	{
	  if (*cp == ' ')
	    indent++;
	  else
	    indent += 8;
	}

      if (!*cp)
	{			/* All blanks, possibly with "{" */
	  puts (buf);
	  continue;
	}
      if (*cp == '{')

        {
	  do_blanks (indent);
	  putchar ('{');
	  ++cp;
	  while (*cp == ' ' || *cp == '\t')
	    ++cp;
	  blanks_done = true;
	  if (!*cp)
	    {
	      putchar ('\n');
	      continue;
	    }
	}

      if (!blanks_done)
	do_blanks (indent);

      if (strncmp (cp, "read ( input", 12) == 0)
	{
	  char variable_name[20];
	  if (sscanf (cp, "read ( input , %s )", variable_name) != 1)
            {
  	      fprintf (stderr, "sscanf failed\n");
              exit (1);
            }
	  printf ("%s = getint();\n", variable_name);
	  continue;
	}

      if (strncmp (cp, "lab", 3) == 0 && strchr (cp, ':'))
	{
	  do
	    {
	      putchar (*cp);
	    }
          while (*cp++ != ':');

          while (*cp == ' ')
	    ++cp;
	  putchar (' ');
	}

      if (strncmp (cp, "else write", 10) == 0)
	{
	  puts ("else");
	  do_blanks (indent);
	  cp += 5;
	  while (*cp == ' ')
	    ++cp;
	}

      if (bare (cp, '{'))
	{
	  while (*cp != '{')
	    {
	      putchar (*cp);
	      ++cp;
	    }
	  ++cp;
	  puts ("{");
	  indent += 4;
	  do_blanks (indent);
	  while (*cp == ' ')
	    ++cp;
	}

      if (strncmp (cp, "write (", 7) && strncmp (cp, "writeln (", 9))
	{
	  /* if not a write/writeln, just copy it to stdout and continue */
	  puts (cp);
	  continue;
	}
      cmd = cp;
      while (!whole (buf))	/* make sure we have whole stmt */
	{
	  if (!fgets (&buf[strlen (buf)], BUFSIZ - strlen (buf), stdin))
            break;
	  remove_newline (buf);
	}

      while (*cp != '(')
	++cp;
      ++cp;
      while (*(cp + 1) == ' ')
	++cp;

      /* Some writes start with a variable, instead of a file. */
      if (*(cp + 1) == '"' || *(cp + 1) == '\''
          || strncmp (cp + 1, "buffer", 6) == 0
          || strncmp (cp + 1, "xchr", 4) == 0
          || strncmp (cp + 1, "k ,", 3) == 0
          || strncmp (cp + 1, "s ,", 3) == 0
          || strncmp (cp + 1, "dig", 3) == 0
          || strncmp (cp + 1, "HEX", 3) == 0
          || strncmp (cp + 1, "versionstring", 13) == 0
          || strncmp (cp + 1, "kpathseaversionstring", 21) == 0
         )
	strcpy (filename, "stdout");
      else
	{
	  file = filename;
	  while (*cp != ',' && *cp != ')')
	    *file++ = *cp++;
	  *file = '\0';
	}
      if (*cp == ')')
	{
	  printf ("putc ('\\n', %s);\n", filename);
	  continue;
	}
      argp = ++cp;
      as = args;
      while (*cp == ' ')
	++cp;
      while (*cp != ')')
	{
	  if (*cp == '\'' || strncmp (cp, "xchr", 4) == 0
              || (strncmp (cp ,"HEX", 3) == 0
                  && (STREQ (program_name, "ofm2opl")
                      || STREQ (program_name, "opl2ofm")
                      || STREQ (program_name, "ovp2ovf")
                      || STREQ (program_name, "ovf2ovp")))
	      || strncmp (cp, "ASCII04", 7) == 0
	      || strncmp (cp, "ASCII1", 6) == 0
	      || strncmp (cp, "ASCIIall", 8) == 0              
	      || strncmp (cp, "months", 6) == 0
	      || strncmp (cp, "nameoffile", 10) == 0
	      || (strncmp (cp, "buffer", 6) == 0
                  && (STREQ (program_name, "vptovf")
                      || STREQ (program_name, "pltotf")
                      || STREQ (program_name, "ppltotf")
                      || STREQ (program_name, "uppltotf")
                      || STREQ (program_name, "ovp2ovf")
                      || STREQ (program_name, "opl2ofm")))
              || (((strncmp (cp, "buf", 3) == 0
		    || strncmp (cp, "xdig", 4) == 0
		    || strncmp (cp, "xext", 4) == 0
		    || strncmp (cp, "xhyf", 4) == 0)
                  && STREQ (program_name, "patgen")))
             )
	    {
	      *as++ = '%';
	      *as++ = 'c';
	      if (tex && strncmp (cp, "xchr", 4) == 0)
		{
		  *cp = 'X';
		  cp = strchr (cp, '[');
		  *cp = '(';
		  cp = advance_cp(cp,1);
		  *cp++ = ')';
		}
	      else if (*cp == '\'')
		cp += 2;
	    }
          
	  else if (*cp == '"')
	    {
	      *as++ = '%';
	      *as++ = 's';
	      while (*++cp != '"')	/* skip to end of string */
		if (*cp == '\\')
		  ++cp;		/* allow \" in string */
	    }

          /* More kludges -- recognize some things as strings and use %s:
             - versionstring
             - poolname
             - formatengine
             - dumpname */
          else if (strncmp (cp, "versionstring", 13) == 0
                   || strncmp (cp, "poolname", 8) == 0
                   || strncmp (cp, "formatengine", 12) == 0
                   || strncmp (cp, "dumpname", 8) == 0)
            {
              *as++ = '%';
              *as++ = 's';
            }

	  /* And yet another kludge, to handle stringcast (<whatever>) */
          else if (strncmp (cp, "stringcast", 10) == 0
                   || strncmp (cp, "conststringcast", 15) == 0)
	    {
	      *as++ = '%';
	      *as++ = 's';
	      cp = skip_balanced (cp);  /* Skip cast expression */
	    }

          else
	    {
	      *as++ = '%';
	      *as++ = 'l';
	      *as++ = 'd';
	      cp = insert_long (cp);
	      cp = skip_balanced (cp);	/* It's a numeric expression */
	    }
	  while (*cp != ',' && *cp != ')')
	    ++cp;
	  while (*cp == ',' || *cp == ' ')
	    ++cp;
	}

      if (strncmp (cmd, "writeln", 7) == 0)
	{
	  *as++ = '\\';
	  *as++ = 'n';
	}

      *as = '\0';
      if (strcmp (args, "%c") == 0)
	{
	  for (as = argp; *as; ++as) ;
	  while (*--as != ')') ;
	  *as = '\0';
	  printf ("putc (%s, %s);\n", argp, filename);
	}
      else if (strcmp (args, "%c\\n") == 0)
	{
	  for (as = argp; *as; ++as) ;
	  while (*--as != ')') ;
	  *as = '\0';
	  printf ("{ putc (%s, %s);  ", argp, filename);
	  printf ("putc ( '\\n', %s); }\n", filename);
	}
      else if (strcmp (args, "%s") == 0)
        printf ("Fputs (%s, %s\n", filename, argp);
      else
        printf ("fprintf (%s, \"%s\", %s\n", filename, args, argp);
    }

  return EXIT_SUCCESS;
}
