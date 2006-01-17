#include <win32lib.h>

#undef _DEBUG

#ifdef _DEBUG
#define TRACE(x) x
#else
#define TRACE(x)
#endif

/*
  Return a quoted string if space is found
  New string is allocated
*/
static char *
quote_elt(char *elt) 
{
  int len = 2*strlen(elt)*sizeof(char) + 2;
  char *to = xmalloc(len);
  char *p = to;
  BOOL need_quote = FALSE;

  if (strchr(elt, ' ')) {
    *to++ = '"';
    need_quote = TRUE;
  }

  while (*elt) {
    switch (*elt) {
    case '"':
    case '\'':
      /*    case '\\': */
      *to++ = '\\';
    default:
      *to++ = *elt++;
    }
  }
  if  (need_quote) {
    *to++ = '"';
  }

  *to = *elt;
  to = xstrdup(p);
  free(p);

  return to;
}

/*
  Process an array of strings, quote them
  an return their catenation
*/
static char *
quote_args(char **argv)
{
  int i;
  char *line = NULL, *new_line;
  char *new_argv_i;

  if (!argv)
    return line;

  line = quote_elt(argv[0]);
  for (i = 1; argv[i]; i++) {
    new_argv_i = quote_elt(argv[i]);
    new_line = concat3(line, " ", new_argv_i);
    TRACE(fprintf(stderr, "quote_args: new_line = %s\n", new_line));
    free(line);
    free(new_argv_i);
    line = new_line;
  }

  return line;
}

int
main (int argc, char *argv[])
{
  char *cmd_line;
  argv[0] = "mktextfm";
  cmd_line = quote_args(argv);
  return system(cmd_line);
}
