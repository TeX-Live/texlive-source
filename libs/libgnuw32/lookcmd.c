/* libc replacement functions for win32.

Copyright (C) 1998, 99 Free Software Foundation, Inc.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

/*
  This does make sense only under WIN32.
  Functions:
    - look_for_cmd() : locates an executable file
    - parse_cmd_line() : splits a command with pipes and redirections
    - build_cmd_line() : builds a command with pipes and redirections (useful ?)
  */

/*
  This part looks for the real location of the program invoked
  by cmd. If it can find the program, that's good. Else 
  command processor is invoked.
*/

#include <win32lib.h>
#include <errno.h>

#undef _DEBUG

#ifdef _DEBUG
#define TRACE(x) x
#else
#define TRACE(x)
#endif

#define DIR_SEP '/'
#define DIR_SEP_STRING "/"
#define IS_DEVICE_SEP(ch) ((ch) == ':')
#define NAME_BEGINS_WITH_DEVICE(name) (*(name) && IS_DEVICE_SEP((name)[1]))
/* On DOS, it's good to allow both \ and / between directories.  */
#define IS_DIR_SEP(ch) ((ch) == '/' || (ch) == '\\')
/* On win32, UNC names are authorized */
#define IS_UNC_NAME(name) (strlen(name)>=3 && IS_DIR_SEP(*name)  \
                            && IS_DIR_SEP(*(name+1)) && isalnum(*(name+2)))

#if (defined(_DEBUG) || defined(TEST))
void display_cmd(char ***cmd, char *input, char *output, char *errput);
#endif

void
free_cmd_pipe(char ***cmd_pipe, int n)
{
  int i,j;
  if (cmd_pipe) {
    for (i = 0; i < n; i++) {
      for (j = 0; cmd_pipe[i][j]; j++) {
	TRACE(fprintf(stderr, "freeing %x\n", cmd_pipe[i][j]));
	free(cmd_pipe[i][j]);
      }
      TRACE(fprintf(stderr, "freeing %x\n", cmd_pipe[i]));
      free(cmd_pipe[i]);
    }
    TRACE(fprintf(stderr, "freeing %x\n", cmd_pipe));
    free(cmd_pipe);
  }
}

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
char *
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

static BOOL
is_absolute(const char *filename)
{
  BOOL absolute = IS_DIR_SEP (*filename)
    /* Novell allows non-alphanumeric drive letters. */
    || (*filename && IS_DEVICE_SEP (filename[1]))
    /* UNC names */
    || IS_UNC_NAME(filename)
    ;
  BOOL explicit_relative = (*filename == '.' && (IS_DIR_SEP (filename[1])
						 || (filename[1] == '.' && IS_DIR_SEP (filename[2]))));
  /* FIXME: On UNIX an IS_DIR_SEP of any but the last character in the name
     implies relative.  */
  return absolute || explicit_relative;
}

BOOL 
look_for_cmd(const char *cmd, char **app)
{
  char *env_path = NULL;
  char *p, *q;
  char pname[MAXPATHLEN];
  char *suffixes[] = {".com", ".exe", ".bat", ".cmd", NULL };
  char **s;
  char *app_name = NULL, *bare_name = NULL;
  int i;
  BOOL bIsWindowsNT = (GetVersion() < 0x80000000);
  BOOL found, has_suffix = FALSE;

  *app = app_name = bare_name = NULL;

  /* We should look for the application name along the PATH,
     and decide to prepend "%COMSPEC% /c " or not to the command line.
     Do nothing for the moment. */

  /* Another way to do that would be to try CreateProcess first without
     invoking cmd, and look at the error code. If it fails because of
     command not found, try to prepend "cmd /c" to the cmd line.
  */

  /* Look for the application name */
  for (p = (char *)cmd; *p && isspace(*p); p++); /* Trim spaces on the left */
  if (*p == '"') {		/* If quoted string */
    q = ++p;
    while(*p && *p != '"') p++;	/* stop at next quote */
    if (*p == '\0') {
      fprintf(stderr, "Look_for_cmd: malformed command (\" not terminated)\n");
      return FALSE;
    }
  }
  else {
    for (q = p; *p && !isspace(*p); p++); /* stop at next space */
  }

  /* q points to the beginning of appname, p to the last + 1 char */
  if ((app_name = xmalloc(p - q + 1)) == NULL) {
    fprintf(stderr, "Look_for_cmd: malloc(app_name) failed.\n");
    return FALSE;
  }
  /* the app name has been found */
  for (i = 0; i < p - q; i++)
    app_name[i] = (q[i] == '/' ? '\\' : q[i]);
  //  strncpy(app_name, q, p - q );
  app_name[p - q] = '\0';

  /* First of all : if it is an absolute path ... */
  if (is_absolute(app_name)) {
    if (_access(app_name, 0) == 0) { /* and it exists */
      *app = app_name;
      found = TRUE;
    }
    else {			/* not found */
      found = FALSE;
    }
    goto exit_1;
  }

  /* pname[] will serve as full path name */
  pname[0] = '\0';
  /* app_name without potential extension */
  bare_name = xstrdup(app_name);
  has_suffix = ((p = strrchr(bare_name, '.')) != NULL);

  TRACE(fprintf(stderr, "look for command: app_name = %s\n", app_name));

  /* Add the current directory */
  {
    char *tmp = getenv("PATH");
    env_path = xmalloc(strlen(tmp) + 3);
    strcpy(env_path, ".;");
    strcat(env_path, tmp);
  }

  /* Looping over directories in env_path */
  p = strtok(env_path, ";");
  found = FALSE;
  while (p != NULL && ! found) {
    /* If the file has an extension ... */
    if (has_suffix) {
      strcpy(pname, p);
      strcat(pname, "\\");
      strcat(pname, app_name);
      TRACE(fprintf(stderr, "look for command: trying %s\n", pname));
      if (_access(pname, 0) == 0) {
	free(app_name);
	app_name = xstrdup(pname);
	found = TRUE;
	break;
      }
    }
    else {
      /* For each suffix in the list */
      for (s = suffixes; *s != NULL; *s++) {
	strcpy(pname, p);
	strcat(pname, "\\");
	strcat(pname, bare_name);
	strcat(pname, *s);
	TRACE(fprintf(stderr, "look for command: trying %s\n", pname));
	if (_access(pname, 0) == 0) {
	  free(app_name);
	  app_name = xstrdup(pname);
	  found = TRUE;
	  break;
	}
      }
    }
    p = strtok(NULL, ";");
  }

 exit_1:

  if (found == TRUE) {
    TRACE(fprintf(stderr, "look for command: app_name = %s\n", app_name));
    if (! bIsWindowsNT) {
      // Fix against short pathnames vs long pathnames
      char lpShortPathName[_MAX_PATH];
      if (GetShortPathName((LPCTSTR)app_name, lpShortPathName, sizeof(lpShortPathName)) != 0) {
	free(app_name);
	app_name = xstrdup(lpShortPathName);
      }
    }
    for (p = app_name; p && *p; p++) {
      *p = (*p == '/' ? '\\' : *p);
    }
    TRACE(fprintf(stderr, "\n!!! Absolute path = %s\n", app_name));
    *app = app_name;
  }
  else {
    /* the app_name was not found */
    TRACE(fprintf(stderr, "look for command: %s not found\n", app_name));
    if (app_name) free(app_name);
    *app = app_name = NULL;
  }

  if (env_path) free(env_path);
  if (bare_name) free(bare_name);

  return found;
}

/*
  Command parser. Borrowed from DJGPP.
 */

static BOOL __system_allow_multiple_cmds = FALSE;

typedef enum {
  EMPTY,
  WORDARG,
  REDIR_INPUT,
  REDIR_APPEND,
  REDIR_OUTPUT,
  PIPE,
  SEMICOLON,
  UNMATCHED_QUOTE,
  EOL
} cmd_sym_t;

/* Return a copy of a word between BEG and (excluding) END with all
   quoting characters removed from it.  */

static char *
__unquote (char *to, const char *beg, const char *end)
{
  const char *s = beg;
  char *d = to;
  int quote = 0;

  while (s < end)
    {
      switch (*s)
	{
	case '"':
	case '\'':
	  if (!quote)
	    quote = *s;
	  else if (quote == *s)
	    quote = 0;
	  s++;
	  break;
	case '\\':
	  if (s[1] == '"' || s[1] == '\''
	      || (s[1] == '&'
		  && (__system_allow_multiple_cmds)))
	    s++;
	  /* Fall-through.  */
	default:
	  *d++ = *s++;
	  break;
	}
    }

  *d = 0;
  TRACE(fprintf(stderr, "unquote returns `%s' (%x)\n", to, to));
  return to;
}

/* A poor-man's lexical analyzer for simplified command processing.

   It only knows about these:

     redirection and pipe symbols
     semi-colon `;' (that possibly ends a command)
     argument quoting rules with quotes and `\'
     whitespace delimiters of words (except in quoted args)

   Returns the type of next symbol and pointers to its first and (one
   after) the last characters.

   Only `get_sym' and `unquote' should know about quoting rules.  */

static cmd_sym_t
get_sym (char *s, char **beg, char **end)
{
  int in_a_word = 0;

  while (isspace (*s))
    s++;

  *beg = s;
  
  do {
    *end = s + 1;

    if (in_a_word
	&& (!*s || strchr ("<>| \t\n", *s)
	    || ((__system_allow_multiple_cmds) && *s == '&')))
      {
	--*end;
	return WORDARG;
      }

    switch (*s)
      {
      case '<':
	return REDIR_INPUT;
      case '>':
	if (**end == '>')
	  {
	    ++*end;
	    return REDIR_APPEND;
	  }
	return REDIR_OUTPUT;
      case '|':
	return PIPE;
      case '&':
	if (__system_allow_multiple_cmds)
	  return SEMICOLON;
	else
	  in_a_word = 1;
	break;
      case '\0':
	--*end;
	return EOL;
      case '\\':
	if (s[1] == '"' || s[1] == '\''
	    || (s[1] == '&' && (__system_allow_multiple_cmds)))
	  s++;
	in_a_word = 1;
	break;
      case '\'':
      case '"':
	{
	  char quote = *s++;

	  while (*s && *s != quote)
	    {
	      if (*s++ == '\\' && (*s == '"' || *s == '\''))
		s++;
	    }
	  *end = s;
	  if (!*s)
	    return UNMATCHED_QUOTE;
	  in_a_word = 1;
	  break;
	}
      default:
	in_a_word = 1;
	break;
      }

    s++;

  } while (1);
}

/*
  What we allow :
  [cmd] [arg1] ... [argn] < [redinput] | [cmd2] | ... | [cmdn] > [redoutput]
*/
void *parse_cmdline(char *line, char **input, char **output, int *nb_cmds)
{
  BOOL again, needcmd = TRUE, bSuccess = TRUE, append_out = FALSE;
  char *beg = line, *end, *new_end;
  cmd_sym_t token = EMPTY, prev_token = EMPTY;
  int ncmd = 0, narg = 0;
  char **fp;
  char ***cmd;
  char *dummy_input;			/* So that we could pass NULL */
  char *dummy_output;			/* instead of a real ??put */

#if 0
  /* if _DEBUG is not defined, these macros will result in nothing. */
   SETUP_CRTDBG;
   /* Set the debug-heap flag so that freed blocks are kept on the
    linked list, to catch any inadvertent use of freed memory */
   SET_CRT_DEBUG_FIELD( _CRTDBG_CHECK_ALWAYS_DF );
   SET_CRT_DEBUG_FIELD( _CRTDBG_ALLOC_MEM_DF );
#endif
  if (input == NULL) input = &dummy_input;
  if (output == NULL) output = &dummy_output;

  *input = NULL;
  *output = NULL;
  cmd = xmalloc(MAX_PIPES*sizeof(char **));
  TRACE(fprintf(stderr, "cmd = %x, size = %d\n", cmd, MAX_PIPES*sizeof(char **)));
  cmd[ncmd] = NULL;

  TRACE(fprintf(stderr, "line = %s\n", line));

  do {
    again = FALSE;
    prev_token = token;
    token = get_sym (beg, &beg, &end);	/* get next symbol */

    TRACE(fprintf(stderr, "token = %s\n", beg));

    switch (token) {
    case WORDARG:
      TRACE(fprintf(stderr, "WORDARG: "));
      if (prev_token == REDIR_INPUT
	  || prev_token == REDIR_OUTPUT) {
	fprintf(stderr, "Ambigous input/output redirect.");
	bSuccess = FALSE;
	goto leave;
      }
      /* Incrementing number of arguments */
      narg++;
      /* First word we see is the program to run.  */
      if (needcmd) {
	cmd[ncmd] = xmalloc(narg * sizeof(char *));
	TRACE(fprintf(stderr, "cmd[%d] = %x, size = %d\n", ncmd, cmd[ncmd], narg*sizeof(char *)));
	if (cmd[ncmd] == NULL) {
	  fprintf(stderr, "parse_cmdline: xmalloc(%d) failed.\n", narg * sizeof(char *));
	}
	needcmd = FALSE;
      }
      else {
	TRACE(fprintf(stderr, "cmd[%d] = %x -> ", ncmd, cmd[ncmd]));
	cmd[ncmd] = xrealloc(cmd[ncmd], narg * sizeof(char *));
	TRACE(fprintf(stderr, "%x, size = %d\n", cmd[ncmd], narg*sizeof(char *)));
	if (cmd[ncmd] == NULL) {
	  fprintf(stderr, "parse_cmdline: xrealloc(%d) failed.\n", narg * sizeof(char *));
	}
      }
      cmd[ncmd][narg - 1] = xmalloc(end - beg + 10);
      TRACE(fprintf(stderr, "cmd[%d][%d] = %x, size = %d\n", ncmd, narg-1, cmd[ncmd][narg-1], end-beg+1));
      if (cmd[ncmd] == NULL) {
	fprintf(stderr, "parse_cmdline: xmalloc(%d) failed.\n", end - beg + 1);
      }
      __unquote (cmd[ncmd][narg - 1], beg, end); /* unquote and copy to prog */
      TRACE(fprintf(stderr, "parse_cmd_line: cmd[%d][%d] = %s\n", ncmd, narg - 1, cmd[ncmd][narg - 1]));
      if (cmd[ncmd][narg - 1][0] == '(') {
	fprintf(stderr, "parse_cmdline(%s): Parenthesized groups not allowed.\n", line);
	bSuccess = FALSE;
	goto leave;
      }
      beg = end; /* go forward */
      again = TRUE;
      break;

    case REDIR_INPUT:
    case REDIR_OUTPUT:
    case REDIR_APPEND:
      TRACE(fprintf(stderr, "REDIR: "));
      if (token == REDIR_INPUT) {
	if (*input) {
	  fprintf(stderr, "Ambiguous input redirect.");
	  errno = EINVAL;
	  bSuccess = FALSE;
	  goto leave;
	}
	fp = input;
      }
      else if (token == REDIR_OUTPUT || token == REDIR_APPEND) {
	if (*output) {
	  fprintf(stderr, "Ambiguous output redirect.");
	  errno = EINVAL;
	  bSuccess = FALSE;
	  goto leave;
	}
	fp = output;
	if (token == REDIR_APPEND)
	  append_out = TRUE;
      }
      if (get_sym (end, &end, &new_end) != WORDARG) {
	fprintf(stderr, "Target of redirect is not a filename.");
	errno = EINVAL;
	bSuccess = FALSE;
	goto leave;
      }
      *fp = (char *)xmalloc (new_end - end + 1);
      if (*fp == NULL) {
	fprintf(stderr, "parse_cmdline: xmalloc(%d) failed.\n", new_end - end + 1);
      }
      __unquote (*fp, end, new_end);
      /* (*fp)[new_end - end] = '\0'; */
      beg = new_end;
      again = TRUE;
      break;
    case PIPE:
      TRACE(fprintf(stderr, "PIPE: "));
      if (*output) {
	fprintf(stderr, "Ambiguous output redirect.");
	errno = EINVAL;
	bSuccess = FALSE;
	goto leave;
      }
      narg++;
      TRACE(fprintf(stderr, "cmd[%d] = %x -> ", ncmd, cmd[ncmd]));
      cmd[ncmd] = xrealloc(cmd[ncmd], narg * sizeof(char *));
      TRACE(fprintf(stderr, "%x, size = %d\n", cmd[ncmd], narg*sizeof(char *)));
      cmd[ncmd][narg - 1] = NULL;
      ncmd++; 
      needcmd = TRUE;
      cmd[ncmd] = NULL;
      beg = end;
      again = TRUE;
      break;
    case SEMICOLON:
    case EOL:
      TRACE(fprintf(stderr, "SEMICOLON|EOL: "));
      if (needcmd) {
	fprintf(stderr, "No command name seen.");
	errno = EINVAL;
	bSuccess = FALSE;
	goto leave;
      }
      narg++;
      TRACE(fprintf(stderr, "cmd[%d] = %x -> ", ncmd, cmd[ncmd]));
      cmd[ncmd] = xrealloc(cmd[ncmd], narg * sizeof(char *));
      TRACE(fprintf(stderr, "%x, size = %d\n", cmd[ncmd], narg*sizeof(char *)));
      cmd[ncmd][narg - 1] = NULL;
      ncmd++;
      cmd[ncmd] = NULL;
      again = FALSE;
      break;
	  
    case UNMATCHED_QUOTE:
      fprintf(stderr, "Unmatched quote character.");
      errno = EINVAL;
      bSuccess = FALSE;
      goto leave;
    default:
      fprintf(stderr, "I cannot grok this.");
      errno = EINVAL;
      bSuccess = FALSE;
      goto leave;

    }

  }	while (again);

 leave:
  if (!bSuccess) {
    int i;
    char **p;
    /* Need to free everything that was allocated */
    for (i = 0; i < ncmd; i++) {
      for (p = cmd[i]; *p; p++) 
	free(*p);
      free(cmd[i]);
    }
    if (cmd[ncmd]) {
      for (i = 0; i < narg; i++)
	free(cmd[ncmd][i]);
      free(cmd[ncmd]);
    }
    free(cmd);
    *cmd = NULL;
    ncmd = 0;
  }

  if (nb_cmds != NULL) *nb_cmds = ncmd;

#if (defined(_DEBUG) || defined(TEST))
  display_cmd(cmd, *input, *output, NULL);
#endif
  return cmd;
}

char *
build_cmdline(char ***cmd, char *input, char *output)
{
  int ncmd;
  char *line = NULL, *new_line;

  if (!cmd)
    return line;

  line = quote_args(cmd[0]);
  if (input) {
    new_line = concat3(line, " < ", quote_elt(input));
    free(line);
    line = new_line;
  }
  for(ncmd = 1; cmd[ncmd]; ncmd++) {
    new_line = concat3(line, " | ", quote_args(cmd[ncmd]));
    free(line);
    line = new_line;
  }

  if (output) {
    new_line = concat3(line, " > ", quote_elt(output));
    free(line);
    line = new_line;
  }

  return line;
}

#if (defined(_DEBUG) || defined(TEST))
void 
display_cmd(char ***cmd, char *input, char *output, char *errput)
{
  int ncmd = 0, narg;
  
  if (cmd) {
    for (ncmd = 0; cmd[ncmd]; ncmd++) {
      printf("cmd[%d] = %s ", ncmd, cmd[ncmd][0]);
      for(narg = 1; cmd[ncmd][narg] != NULL; narg++)
	printf("%s ", cmd[ncmd][narg]);
      printf("\n");
    }
  }
  printf("input = %s\n", (input ? input : "<stdin>"));
  printf("output = %s\n", (output ? output : "<stdout>"));
  printf("errput = %s\n", (errput ? errput : "<stderr>"));
}
#endif

#ifdef TEST
main(int argc, char *argv[])
{
  char *input;
  char *output;
  char ***cmd;
  char *line;

  /*  SETUP_CRTDBG; */

  printf("%s\n", line = "foo a b");
  if (cmd = parse_cmdline(line, &input, &output, 0)) {
    display_cmd(cmd, input, output, NULL);
  }
  printf("%s\n", line = "foo < a > b");
  if (cmd = parse_cmdline(line, &input, &output, 0)) {
    display_cmd(cmd, input, output, NULL);
  }
  printf("%s\n", line = "filter 1 | filter 2");
  if (cmd = parse_cmdline(line, &input, &output, 0)) {
    display_cmd(cmd, input, output, NULL);
  }
  printf("%s\n", line = "foo > a < b");
  if (cmd = parse_cmdline(line, &input, &output, 0)) {
    display_cmd(cmd, input, output, NULL);
  }
  printf("%s\n", line = "foo | a b");
  if (cmd = parse_cmdline(line, &input, &output, 0)) {
    display_cmd(cmd, input, output, NULL);
  }
  printf("%s\n", line = "foo a | b");
  if (cmd = parse_cmdline(line, &input, &output, 0)) {
    display_cmd(cmd, input, output, NULL);
  }
  printf("%s\n", line = "foo | a > b");
  if (cmd = parse_cmdline(line, &input, &output, 0)) {
    display_cmd(cmd, input, output, NULL);
  }
  printf("%s\n", line = "foo < a | b");
  if (cmd = parse_cmdline(line, &input, &output, 0)) {
    display_cmd(cmd, input, output, NULL);
  }
  printf("%s\n", line = "foo < a | b");
  if (cmd = parse_cmdline(line, &input, &output, 0)) {
    display_cmd(cmd, input, output, NULL);
  }
  printf("%s\n", line = "foo < a | b b1 b2 | c > d");
  if (cmd = parse_cmdline(line, &input, &output, 0)) {
    display_cmd(cmd, input, output, NULL);
  }

  printf("%s\n", line = "foo > a | b < c");
  if (cmd = parse_cmdline(line, &input, &output, 0)) {
    display_cmd(cmd, input, output, NULL);
  }
  printf("%s\n", line = "\"C:\\Program Files\\WinEdt\\WinEdt.exe\" -F \"[Open('%f');SelLine(%l,8)]\"");
  if (cmd = parse_cmdline(line, &input, &output, 0)) {
    display_cmd(cmd, input, output, NULL);
  }

  {
    char buf[256];
    char *app, *cmd;
    while (gets(buf)) {
      look_for_cmd(buf, &app, &cmd);
      if (app) {
	printf("app: %s, cmd: %s\n", app, cmd);
      }
      else {
	printf("app not found, cmd: %s\n", cmd);
      }
      if (app) free(app);
      if (cmd) free(cmd);
    }
  }
}
#endif
