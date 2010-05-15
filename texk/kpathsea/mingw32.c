/* mingw32.c: bits and pieces for mingw32

   Copyright 2009, 2010 Taco Hoekwater <taco@luatex.org>.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this library; if not, see <http://www.gnu.org/licenses/>.
*/

/* Original sources lifted from the distribution of XEmacs for Windows NT,
   Copyright 1994-1996 Free Software Foundation, later adapted to
   fpTeX 0.4 (2000) by Fabrice Popineau <Fabrice.Popineau@supelec.fr>,
   then simplified and re-adapted to TeXLive (2009) by Taco Hoekwater
   <taco@luatex.org>.
*/

#ifdef __MINGW32__

#include <kpathsea/config.h>
#include <kpathsea/c-proto.h>
#include <kpathsea/mingw32.h>
#include <kpathsea/lib.h>
#include <kpathsea/concatn.h>
#include <shlobj.h>
#include <errno.h>

/* Emulate getpwuid, getpwnam and others.  */

typedef HWND (WINAPI *pGetDesktopWindow)(void);

typedef HRESULT (WINAPI * pSHGetSpecialFolderPathA)(HWND, LPSTR, int, BOOL);

extern int __cdecl _set_osfhnd (int fd, long h);
extern int __cdecl _free_osfhnd (int fd);

static char *get_home_directory (void);
static int _parse_root (char * name, char ** pPath);

void
init_user_info (void)
{
  /* Ensure HOME and SHELL are defined. */
  char *home = get_home_directory();
  if (home) {
    putenv(concat("HOME=", home));
  }
  else {
    putenv ("HOME=c:/");
  }
  if (getenv ("SHELL") == NULL)
    putenv ((GetVersion () & 0x80000000) ? "SHELL=command" : "SHELL=cmd");

  {
    /* Win2K problem : we need a specific TEMP directory with
       full access rights so that any user building a format file
       or a font file will build it with full access rights. The installer
       takes care of defining TEXMFTEMP=$SELFAUTOPARENT/tmp in the environment.
       If it is defined, then use it as the TEMP and TMP variables.
    */
    char *p;
    if ((p = getenv("TEXMFTEMP")) != NULL) {
      putenv(concat("TEMP=", p));
      putenv(concat("TMP=", p));
    }
  }
}

/* Returns the home directory, in external format */
static char *
get_home_directory (void)
{
    char *found_home_directory = NULL;

  if ((found_home_directory = getenv("HOME")) != NULL) {
	char q[MAXPATHLEN];
	/* In case it is %HOMEDRIVE%%HOMEPATH% */
	if (ExpandEnvironmentStrings(found_home_directory, q, sizeof(q)) == 0) {
	  /* Error */
	  found_home_directory = NULL;
	}
	else {
	  found_home_directory = xstrdup(q);
	  goto done;
	}
  }

  {
	char	*homedrive, *homepath;
	if ((homedrive = getenv("HOMEDRIVE")) != NULL &&
		(homepath = getenv("HOMEPATH")) != NULL) {
	  found_home_directory = concat(homedrive, homepath);
	  goto done;
	}
  }

  /* This method is the prefered one because even if it requires a more recent shell32.dll,
     it does not need to call SHMalloc()->Free() */
  {
	/* This will probably give the wrong value */
	char q [MAXPATHLEN];
	HINSTANCE h;
    pSHGetSpecialFolderPathA p1;
    pGetDesktopWindow p2;
	HWND hwnd = NULL;

	if ((h = LoadLibrary("user32.dll"))) {
	  if ((p2 = (pGetDesktopWindow)GetProcAddress(h, "GetDesktopWindow")))
	    hwnd = (*p2)();
	  FreeLibrary(h);
	}
	
	if (hwnd && (h = LoadLibrary("shell32.dll"))) {
	  if ((p1 = (pSHGetSpecialFolderPathA)GetProcAddress(h, "SHGetSpecialFolderPathA")))
	    if ((*p1)(hwnd, q, CSIDL_PERSONAL, TRUE)) {
	      found_home_directory = xstrdup(q);
	    }
	  FreeLibrary(h);
	}
	if (found_home_directory) goto done;
  }

  if (1) {
	fprintf(stderr, "kpathsea has been unable to determine a good value for the user's $HOME\n"
			"	directory, and will be using the value:\n"
			"		%s\n"
			"	This is probably incorrect.\n",
			found_home_directory
			);
  }
 done:
  return found_home_directory;
}


/* Consider cached volume information to be stale if older than 10s,
   at least for non-local drives.  Info for fixed drives is never stale.  */
#define DRIVE_INDEX( c ) ( (c) <= 'Z' ? (c) - 'A' : (c) - 'a' )
#define VOLINFO_STILL_VALID( root_dir, info )		\
  ( ( isalpha (root_dir[0]) )				\
    || GetTickCount () - info->timestamp < 10000 )


/* Normalize filename by converting all path separators to
   the specified separator.  Also conditionally convert upper
   case path name components to lower case.
   Returns the index of the first meaningful char in the path
   past any drive specifier of unc name specifier.
   Remove any multiple path separators after a leading
   drive specifier or double path separator.
*/

static int
normalize_filename (char *fp, char path_sep)
{
  char *p;
  int ret, i;

  /* Always lower-case drive letters a-z, even if the filesystem
     preserves case in filenames.
     This is so filenames can be compared by string comparison
     functions that are case-sensitive.  Even case-preserving filesystems
     do not distinguish case in drive letters.  */
  if (fp[1] == ':' && *fp >= 'A' && *fp <= 'Z') {
    *fp += 'a' - 'A';
  }

  /* Remove unneeded double slashes */
  ret = (IS_UNC_NAME(fp) ? 2 :
	 NAME_BEGINS_WITH_DEVICE(fp) ?
	 (IS_DIR_SEP(*(fp+2)) ? 3 : 2) : IS_DIR_SEP(*fp) ? 1 : 0);
  for (i = ret, p = fp+i;
       IS_DIR_SEP(*p);
       i++, p++);
  if (i > ret) {
    int len = strlen(fp+i);
    /* remove unneeded slashes, for the sake of win95 */
#if 0
    fprintf(stderr, "moving %s to %s\n", fp+ret, fp+i);
#endif
    memcpy(fp+ret, fp+i, len);
    *(char *)(fp+ret+len) = '\0';
  }

  /* conditionnally rewrite to same path_sep, slash preferably */
  if (path_sep) {
    for (p = fp; *p; p++)
      if (IS_DIR_SEP(*p))
	*p = path_sep;
  }

#if 0
    fprintf(stderr, "normalize_filename returned (%d) %s\n", ret, fp);
#endif

  return ret;
}


/* Destructively turn backslashes into slashes.  */
void
dostounix_filename (char *p)
{
  normalize_filename (p, '/');
}

/* Destructively turn slashes into backslashes.  */
static void
unixtodos_filename (char *p)
{
  normalize_filename (p, '\\');
}

/* Remove all CR's that are followed by a LF.
   (From msdos.c...probably should figure out a way to share it,
   although this code isn't going to ever change.)  */
#if 0 /* unused */
static int
crlf_to_lf (int n, unsigned char *buf, unsigned *lf_count)
{
  unsigned char *np = buf;
  unsigned char *startp = buf;
  unsigned char *endp = buf + n;

  if (n == 0)
    return n;
  while (buf < endp - 1)
    {
      if (*buf == 0x0a)
	(*lf_count)++;
      if (*buf == 0x0d)
	{
	  if (*(++buf) != 0x0a)
	    *np++ = 0x0d;
	}
      else
	*np++ = *buf++;
    }
  if (buf < endp)
    {
      if (*buf == 0x0a)
	(*lf_count)++;
    *np++ = *buf++;
    }
  return np - startp;
}
#endif

/* Parse the root part of file name, if present.  Return length and
    optionally store pointer to char after root.  */
static int
_parse_root (char * name, char ** pPath)
{
  char * start = name;

  if (name == NULL)
    return 0;

  /* find the root name of the volume if given */
  if (isalpha (name[0]) && name[1] == ':')
    {
      /* skip past drive specifier */
      name += 2;
      if (IS_DIR_SEP (name[0]))
	name++;
    }
  else if (IS_DIR_SEP (name[0]) && IS_DIR_SEP (name[1]))
    {
      int slashes = 2;
      name += 2;
      do
        {
	  if (IS_DIR_SEP (*name) && --slashes == 0)
	    break;
	  name++;
	}
      while ( *name );
      if (IS_DIR_SEP (name[0]))
	name++;
    }

  if (pPath)
    *pPath = name;

  return name - start;
}

/* Get long base name for name; name is assumed to be absolute.  */
static int
get_long_basename (char * name, char * buf, int size)
{
  WIN32_FIND_DATA find_data;
  HANDLE dir_handle;
  int len = 0;
#ifdef PIGSFLY
  char *p;

  /* If the last component of NAME has a wildcard character,
     return it as the basename.  */
  p = name + strlen (name);
  while (*p != '\\' && *p != ':' && p > name) p--;
  if (p > name) p++;
  if (strchr (p, '*') || strchr (p, '?'))
    {
      if ((len = strlen (p)) < size)
	memcpy (buf, p, len + 1);
      else
	len = 0;
      return len;
    }
#endif

  dir_handle = FindFirstFile (name, &find_data);
  if (dir_handle != INVALID_HANDLE_VALUE)
    {
      if ((len = strlen (find_data.cFileName)) < size)
	memcpy (buf, find_data.cFileName, len + 1);
      else
	len = 0;
      FindClose (dir_handle);
    }
  return len;
}

/* Get long name for file, if possible (assumed to be absolute).  */
BOOL
win32_get_long_filename (char * name, char * buf, int size)
{
  char * o = buf;
  char * p;
  char * q;
  char full[ MAX_PATH ];
  int len;

  len = strlen (name);
  if (len >= MAX_PATH)
    return FALSE;

  /* Use local copy for destructive modification.  */
  memcpy (full, name, len+1);
  unixtodos_filename (full);

  /* Copy root part verbatim.  */
  len = _parse_root (full, &p);
  memcpy (o, full, len);
  o += len;
  size -= len;

  do
    {
      q = p;
      p = strchr (q, '\\');
      if (p) *p = '\0';
      len = get_long_basename (full, o, size);
      if (len > 0)
	{
	  o += len;
	  size -= len;
	  if (p != NULL)
	    {
	      *p++ = '\\';
	      if (size < 2)
		return FALSE;
	      *o++ = '\\';
	      size--;
	      *o = '\0';
	    }
	}
      else
	return FALSE;
    }
  while (p != NULL && *p);

  return TRUE;
}

/*
  This does make sense only under WIN32.
  Functions:
    - look_for_cmd() : locates an executable file
    - parse_cmdline() : splits a command with pipes and redirections
    - build_cmdline() : builds a command with pipes and redirections (useful ?)
  */

/*
  This part looks for the real location of the program invoked
  by cmd. If it can find the program, that's good. Else
  command processor is invoked.
*/

BOOL
look_for_cmd(const char *cmd, char **app)
{
  char *env_path;
  const char *p, *q;
  char pname[MAXPATHLEN], *fp;
  const char *suffixes[] = { ".bat", ".cmd", ".com", ".exe", NULL };
  const char **s;
  char *app_name;

  BOOL go_on;

  *app = NULL;
  app_name = NULL;

  /* We should look for the application name along the PATH,
     and decide to prepend "%COMSPEC% /c " or not to the command line.
     Do nothing for the moment. */

  /* Another way to do that would be to try CreateProcess first without
     invoking cmd, and look at the error code. If it fails because of
     command not found, try to prepend "cmd /c" to the cmd line.
  */

  /* Look for the application name */
  for (p = cmd; *p && isspace(*p); p++);
  if (*p == '"') {
    q = ++p;
    while(*p && *p != '"') p++;
    if (*p == '\0') {
      fprintf(stderr, "Look_for_cmd: malformed command (\" not terminated)\n");
      return FALSE;
    }
  }
  else
    for (q = p; *p && !isspace(*p); p++);
  /* q points to the beginning of appname, p to the last + 1 char */
  if ((app_name = malloc(p - q + 1)) == NULL) {
    fprintf(stderr, "Look_for_cmd: malloc(app_name) failed.\n");
    return FALSE;
  }
  strncpy(app_name, q, p - q );
  app_name[p - q] = '\0';
  pname[0] = '\0';
#ifdef TRACE
  fprintf(stderr, "popen: app_name = %s\n", app_name);
#endif

  {
    char *tmp = getenv("PATH");
    env_path = xmalloc(strlen(tmp) + 3);
    strcpy(env_path, ".;");
    strcat(env_path, tmp);
  }

  /* Looking for appname on the path */
  for (s = suffixes, go_on = TRUE; go_on; *s++) {
    if (SearchPath(env_path,	/* Address of search path */
		   app_name,	/* Address of filename */
		   *s,		/* Address of extension */
		   MAXPATHLEN,	/* Size of destination buffer */
		   pname,	/* Address of destination buffer */
		   &fp)		/* File part of app_name */
	!= 0) {
#ifdef TRACE
      fprintf(stderr, "%s found with suffix %s\nin %s\n", app_name, *s, pname);
#endif
      free(app_name);
      app_name = xstrdup(pname);
      break;
    }
    go_on = (*s != NULL);
  }
  if (go_on == FALSE) {
    /* the app_name was not found */
#ifdef TRACE
    fprintf(stderr, "%s not found, concatenating comspec\n", app_name);
#endif
    free(app_name);
    app_name = NULL;
  }
  if (env_path) free(env_path);

  *app = app_name;

  return TRUE;

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
	      || (s[1] == ';'
		  && (__system_allow_multiple_cmds)))
	    s++;
	  /* Fall-through.  */
	default:
	  *d++ = *s++;
	  break;
	}
    }

  *d = 0;
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
	    || ((__system_allow_multiple_cmds) && *s == ';')))
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
      case ';':
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
	    || (s[1] == ';' && (__system_allow_multiple_cmds)))
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
static void *
parse_cmdline(char *line, char **input, char **output)
{
  BOOL again, needcmd = TRUE, bSuccess = TRUE, append_out = FALSE;
  char *beg = line, *end, *new_end;
  cmd_sym_t token = EMPTY, prev_token = EMPTY;
  int ncmd = 0, narg = 1;
  char **fp = NULL;
  char ***cmd;
  char *dummy_input;			/* So that we could pass NULL */
  char *dummy_output;			/* instead of a real ??put */

  if (input == NULL) input = &dummy_input;
  if (output == NULL) output = &dummy_output;

  *input = NULL;
  *output = NULL;
  cmd = xmalloc(MAX_PIPES*sizeof(char **));
  cmd[ncmd] = NULL;
#ifdef TRACE
  fprintf(stderr, "line = %s\n", line);
#endif
  do {
    again = FALSE;
    prev_token = token;
    token = get_sym (beg, &beg, &end);	/* get next symbol */
#ifdef TRACE
    fprintf(stderr, "token = %s\n", beg);
#endif	
    switch (token) {
    case WORDARG:
      if (prev_token == REDIR_INPUT
	  || prev_token == REDIR_OUTPUT) {
	fprintf(stderr, "Ambigous input/output redirect.");
	bSuccess = FALSE;
	goto leave;
      }
      /* First word we see is the program to run.  */
      if (needcmd) {
	narg = 1;
	cmd[ncmd] = xmalloc(narg * sizeof(char *));
	cmd[ncmd][narg - 1] = xmalloc(end - beg + 1);
	__unquote (cmd[ncmd][narg - 1], beg, end); /* unquote and copy to prog */
	if (cmd[ncmd][narg - 1][0] == '(') {
	  fprintf(stderr, "parse_cmdline(%s): Parenthesized groups not allowed.\n", line);
	  bSuccess = FALSE;
	  goto leave;
	}
	needcmd = FALSE;
      }
      else {
	narg++;
	cmd[ncmd] = xrealloc(cmd[ncmd], narg * sizeof(char *));
	cmd[ncmd][narg - 1] = xmalloc(end - beg + 1);
	__unquote (cmd[ncmd][narg - 1], beg, end); /* unquote and copy to prog */
      }
      beg = end; /* go forward */
      again = TRUE;
      break;

    case REDIR_INPUT:
    case REDIR_OUTPUT:
    case REDIR_APPEND:
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
      memcpy (*fp, end, new_end - end);
      (*fp)[new_end - end] = '\0';
      beg = new_end;
      again = TRUE;
      break;
    case PIPE:
      if (*output) {
	fprintf(stderr, "Ambiguous output redirect.");
	errno = EINVAL;
	bSuccess = FALSE;
	goto leave;
      }
      narg++;
      cmd[ncmd] = xrealloc(cmd[ncmd], narg * sizeof(char *));
      cmd[ncmd][narg - 1] = NULL;
      ncmd++;
      needcmd = TRUE;
      beg = end;
      again = TRUE;
      break;
    case SEMICOLON:
    case EOL:
      if (needcmd) {
	fprintf(stderr, "No command name seen.");
	errno = EINVAL;
	bSuccess = FALSE;
	goto leave;
      }
      narg++;
      cmd[ncmd] = xrealloc(cmd[ncmd], narg * sizeof(char *));
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
  }
  return cmd;
}

static char *
quote_elt(char *elt)
{
  char *p;
  for (p = elt; *p; p++)
    if (isspace(*p))
      return concat3("\"", elt, "\"");

  return xstrdup(elt);
}

char *
quote_args(char **argv)
{
  int i;
  char *line = NULL, *new_line;
  char *new_argv;

  if (!argv)
    return line;

  line = quote_elt(argv[0]);
  for (i = 1; argv[i]; i++) {
    new_argv = quote_elt(argv[i]);
    new_line = concat3(line, " ", new_argv);
    free(line);
    free(new_argv);
    line = new_line;
  }

  return line;
}

static char *
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

/*
  It has been proven that system() fails to retrieve exit codes
  under Win9x. This is a workaround for this bug.
*/

int
win32_system(const char *cmd, int async)
{
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  DWORD ret = 0;
  HANDLE hIn, hOut;
  SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
  int i;

  char  *app_name, *new_cmd;
  char  *red_input, *red_output, ***cmd_pipe;

  /* Reset errno ??? */
  errno = 0;
  hIn = INVALID_HANDLE_VALUE; /* compiler warning */
  hOut = INVALID_HANDLE_VALUE; /* compiler warning */

  /* Admittedly, the command interpreter will allways be found. */
  if (! cmd) {
    errno = 0;
#ifdef _TRACE
    fprintf(stderr, "system: (null) command.\n");
#endif
    return 1;
  }

  if (look_for_cmd(cmd, &app_name) == FALSE) {
    /* Failed to find the command or malformed cmd */
    errno = ENOEXEC;
#ifdef _TRACE
    fprintf(stderr, "system: failed to find command.\n");
#endif
    return -1;
  }

  new_cmd = xstrdup(cmd);
  cmd_pipe = parse_cmdline(new_cmd, &red_input, &red_output);

  for (i = 0; cmd_pipe[i]; i++) {

    /* free the cmd and build the current one */
    if (new_cmd) free(new_cmd);

    new_cmd = build_cmdline(&cmd_pipe[i], NULL, NULL);

    /* First time, use red_input if available */
    if (i == 0) {
      if (red_input) {
	hIn = CreateFile(red_input,
			 GENERIC_READ,
			 FILE_SHARE_READ | FILE_SHARE_WRITE,
			 &sa,
			 OPEN_EXISTING,
			 FILE_ATTRIBUTE_NORMAL,
			 NULL);
	if (hIn == INVALID_HANDLE_VALUE) {
#ifdef _TRACE
	  fprintf(stderr, "system: failed to open hIn (%s) with error %d.\n", red_input, GetLastError());
#endif
	  errno = EIO;
	  return -1;
	}
      }
      else {
	hIn = GetStdHandle(STD_INPUT_HANDLE);
      }
    }
    /* Last time, use red_output if available */
    if (cmd_pipe[i+1] == NULL) {
      if (red_output) {
	hOut = CreateFile(red_output,
			  GENERIC_WRITE,
			  FILE_SHARE_READ | FILE_SHARE_WRITE,
			  &sa,
			  OPEN_ALWAYS,
			  FILE_ATTRIBUTE_NORMAL,
			  NULL);
	if (hOut == INVALID_HANDLE_VALUE) {
#ifdef _TRACE
	  fprintf(stderr, "system: failed to open hOut (%s) with error %d.\n", red_output, GetLastError());
#endif
	  errno = EIO;
	  return -1;
	}
      }
      else {
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
      }
    }

    ZeroMemory( &si, sizeof(STARTUPINFO) );
    si.cb = sizeof(STARTUPINFO);
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_SHOW;
    si.hStdInput = hIn;
    si.hStdOutput = hOut;
    si.hStdError = GetStdHandle(STD_ERROR_HANDLE);

#ifdef _TRACE
    fprintf(stderr, "Executing: %s\n", new_cmd);
#endif
    if (CreateProcess(app_name,
		      new_cmd,
		      NULL,
		      NULL,
		      TRUE,
		      0,
		      NULL,
		      NULL,
		      &si,
		      &pi) == 0) {
      fprintf(stderr, "win32_system(%s) call failed (Error %d).\n", cmd, (int)GetLastError());
      return -1;
    }

    /* Only the process handle is needed */
    CloseHandle(pi.hThread);

    if (async == 0) {
      if (WaitForSingleObject(pi.hProcess, INFINITE) == WAIT_OBJECT_0) {
	if (GetExitCodeProcess(pi.hProcess, &ret) == 0) {
	  fprintf(stderr, "Failed to retrieve exit code: %s (Error %d)\n", cmd, (int)GetLastError());
	  ret = -1;
	}
      }
      else {
        fprintf(stderr, "Failed to wait for process termination: %s (Error %d)\n", cmd, (int)GetLastError());
	ret = -1;
      }
    }

    CloseHandle(pi.hProcess);

    if (red_input) CloseHandle(hIn);
    if (red_output) CloseHandle(hOut);
  }

  if (new_cmd) free(new_cmd);
  if (app_name) free(app_name);

  return ret;
}

#endif /* __MINGW32__ */
