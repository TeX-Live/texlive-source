/* Libc replacement functions for win32.

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
    - popen() rewritten
    - pclose() rewritten
    - stat() wrapper for _stat(), removing trailing slashes
  */

#ifdef 0

#include <fcntl.h>
#include <io.h>
#include <sys/stat.h>
#include <direct.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <gnuw32.h>




#if 0
#endif


#if 0


Thanks.  DJGPP startup code has a full-blown wildcard globber in its 
startup code as well, and it expands wildcards Unix style (i.e., it 
supports [a-z] etc.).  So I don't think we need any more functionality...

Here's the relevant parts from DJGPP's system.c module.  I omitted some 
functions with obvious names like `emiterror', to avoid cluttering the 
code with too many irrelevant details.  `system1' (also omitted) is the 
place where the command line, after being stripped from redirection and 
pipe parts, is actually broken into separate argv[] parts and passed to 
the child (via `spawnve' in the case of DJGPP); I assume that you already 
have something like that.  I did include the two functions which are used 
to support quoting of command-line arguments, in case you are interested, 
but note that some of their subtleties know how the startup code on the 
child side will process quoted arguments (e.g., the case of 'foo bar' 
with single quotes), so caveat emptor.

The only part that is really relevant to processing pipes and redirection 
is the loop which begins with the following lines:

    tmp = alloca(L_tmpnam);
    prog = alloca (L_tmpnam);

    s = strcpy (alloca (strlen (cmdline) + 1), cmdline);
    while (!done && result >= 0)
    {

Have fun!

------------------------------------------------------------------
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
		&& (__system_flags & __system_allow_multiple_cmds)))
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
	    || ((sys_flags & __system_allow_multiple_cmds) && *s == ';')))
    {
      --*end;
      return WORD;
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
	if (sys_flags & __system_allow_multiple_cmds)
	  return SEMICOLON;
	else
	  in_a_word = 1;
	break;
      case '\0':
	--*end;
	return EOL;
      case '\\':
	if (s[1] == '"' || s[1] == '\''
	    || (s[1] == ';' && (sys_flags & __system_allow_multiple_cmds)))
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

/* This function handles redirection and passes the rest to system1
   or to the shell.  */
int
system (const char *cmdline)
{
  /* Set the feature bits for this run: either from the
     environment or from global variable.  */
  const char *envflags = getenv ("DJSYSFLAGS");
  const char *comspec  = getenv ("COMSPEC");
  const char *shell    = 0;
  int call_shell;

  sys_flags = __system_flags;
  if (envflags && *envflags)
  {
    char *stop;
    long flags = strtol (envflags, &stop, 0);

    if (*stop == '\0')
      sys_flags = flags;
  }

  if (sys_flags & __system_use_shell)
    shell = getenv ("SHELL");
  if (!shell)
    shell = comspec;
  if (!shell)
    shell = command_com;

  call_shell =
    (sys_flags & __system_call_cmdproc)
    || (!(sys_flags & __system_emulate_command) && _is_unixy_shell (shell));

  /* Special case: NULL means return non-zero if the command
     interpreter is available.  This is ANSI C requirement.

     If we will call the shell to do everything, we need to see
     whether it exists.  But if most of the work will be done by us
     anyway (like it usually is with stock DOS shell), return non-zero
     without checking, since our emulation is always ``available''.  */
  if (cmdline == 0)
  {
    if (!call_shell)
      return 1;
    else
    {
      char full_path[FILENAME_MAX];

      return __dosexec_find_on_path (shell, (char **)0, full_path) ? 1 : 0;
    }
  }

  /* Strip initial spaces (so that if the command is empty, we
     know it right here).  */
  while (isspace(*cmdline))
    cmdline++;

  /* Call the shell if:

     	the command line is empty
     or
	they want to always do it via command processor
     or
	$SHELL or $COMSPEC point to a unixy shell  */
  if (!*cmdline || call_shell)
    return _shell_command ("", cmdline);
  else
  {
    char *f_in = 0, *f_out = 0;
    int rm_in = 0, rm_out = 0;
    /* Assigned to silence -Wall */
    int h_in = 0, h_inbak = 0, h_out = 0, h_outbak = 0;
    char *s, *t, *u, *v, *tmp, *cmdstart;
    int result = 0, done = 0;
    int append_out = 0;
    cmd_sym_t token;
    char *prog;

    tmp = alloca(L_tmpnam);
    prog = alloca (L_tmpnam);

    s = strcpy (alloca (strlen (cmdline) + 1), cmdline);
    while (!done && result >= 0)
    {
      char **fp = &f_in;
      /* Assignments pacify -Wall */
      int hin_err = 0, hbak_err = 0, hout_err = 0;
      int needcmd = 1;
      int again;

      if (rm_in)
	remove (f_in);
      f_in = f_out;			/* Piping.  */
      rm_in = rm_out;
      f_out = 0;
      rm_out = 0;
      append_out = 0;

      cmdstart = s;

      do {
	again = 0;
	token = get_sym (s, &t, &u);	/* get next symbol */

	/* Weed out extra whitespace, leaving only a single
	   whitespace character between any two tokens.
	   This way, if we eventually pass the command to a
	   shell, it won't fail due to length > 126 chars
	   unless it really *is* that long.  */

	if (s == cmdstart)
	  v = s;	/* don't need blank at beginning of cmdline  */
	else
	  v = s + 1;
	if (t > v)
	{
	  strcpy (v, t);
	  u -= t - v;
	  t = v;
	}

	switch (token)
	{
	case WORD:
	  /* First word we see is the program to run.  */
	  if (needcmd)
	  {
	    __unquote (prog, t, u); /* unquote and copy to prog */
	    /* We can't grok commands in parentheses, so assume they
	       use a shell that knows about these, like 4DOS or `sh'.

	       FIXME: if the parenthesized group is NOT the first command
	       in a pipe, the commands that preceed it will be run twice.  */
	    if (prog[0] == '(')
	      return _shell_command ("", cmdline);
	    strcpy (s, u);	  /* remove program name from cmdline */
	    needcmd = 0;
	  }
	  else
	    s = u;
	  again = 1;
	  break;
	case REDIR_INPUT:
	case REDIR_OUTPUT:
	case REDIR_APPEND:
	  if (!(sys_flags & __system_redirect))
	    return _shell_command ("", cmdline);
	  if (token == REDIR_INPUT)
	  {
	    if (f_in)
	    {
	      result = emiterror ("Ambiguous input redirect.", 0);
	      errno = EINVAL;
	      goto leave;
	    }
	    fp = &f_in;
	  }
	  else if (token == REDIR_OUTPUT || token == REDIR_APPEND)
	  {
	    if (f_out)
	    {
	      result = emiterror ("Ambiguous output redirect.", 0);
	      errno = EINVAL;
	      goto leave;
	    }
	    fp = &f_out;
	    if (token == REDIR_APPEND)
	      append_out = 1;
	  }
	  if (get_sym (u, &u, &v) != WORD)
	  {
	    result = emiterror ("Target of redirect is not a filename.", 0);
	    errno = EINVAL;
	    goto leave;
	  }
	  *fp = memcpy ((char *)alloca (v - u + 1), u, v - u);
	  (*fp)[v - u] = 0;
	  strcpy (t, v);
	  again = 1;
	  break;
	case PIPE:
	  if (!(sys_flags & __system_redirect))
	    return _shell_command ("", cmdline);
	  if (f_out)
	  {
	    result = emiterror ("Ambiguous output redirect.", 0);
	    errno = EINVAL;
	    goto leave;
	  }

	  /* tmpnam guarantees unique names */
	  tmpnam(tmp);
	  f_out = strcpy (alloca (L_tmpnam), tmp);
	  rm_out = 1;
	  /* Fall through.  */
	case SEMICOLON:
	case EOL:
	  if (needcmd)
	  {
	    result = emiterror ("No command name seen.", 0);
	    errno = EINVAL;
	    goto leave;
	  }

	  /* Remove extra whitespace at end of command.  */
	  while (s > cmdstart && isspace (s[-1])) s--;
	  while (t > s && isspace (t[-1])) t--;
	  *t = 0;

#ifdef TEST
	  fprintf (stderr, "Input from: %s\nOutput%s to:  %s\n",
		   f_in ? f_in : "<stdin>",
		   append_out ? " appended" : "",
		   f_out ? f_out : "<stdout>");
	  fflush (stderr);
#endif
	  if (f_in)
	  {
	    int e = errno;

	    errno = 0;
	    h_in = open (f_in, O_RDONLY | O_BINARY);
	    hin_err = errno;
	    errno = 0;
	    h_inbak = dup (0);
	    hbak_err = errno;
	    dup2 (h_in, 0);
	    errno = e;
	  }
	  if (f_out)
	  {
	    int e = errno;

	    errno = 0;
	    h_out = open (f_out,
			  O_WRONLY | O_BINARY | O_CREAT
			  | (append_out ? O_APPEND: O_TRUNC),
			  S_IREAD | S_IWRITE);
	    hout_err = errno;
	    errno = 0;
	    h_outbak = dup (1);
	    hbak_err = errno;
	    fflush(stdout);  /* so any buffered chars will be written out */
	    dup2 (h_out, 1);
	    errno = e;
	  }
	  if (f_in && h_in < 0)
	    result = emiterror ("Cannot redirect input", hin_err);
	  else if ((f_in && h_inbak < 0) || (f_out && h_outbak < 0))
	    result = emiterror ("Out of file handles in redirect", hbak_err);
	  else if (f_out && h_out < 0)
	    result = emiterror ("Cannot redirect output", hout_err);

	  if (!result)
	  {
#ifdef TEST
	    fprintf (stderr, "system1 (\"%s\", \"%s\")\n", prog, cmdstart);
	    fflush (stderr);
#endif
	    /* Tell `__spawnve' it was invoked by `system', so it would
	       know how to deal with command-line arguments' quoting.  */
	    __dosexec_in_system = 1;
	    result = system1 (prog, cmdstart);
	    __dosexec_in_system = 0;
	  }
	  if (f_in)
	  {
	    dup2 (h_inbak, 0);
	    close (h_in);
	    close (h_inbak);
	  }
	  if (f_out)
	  {
	    dup2 (h_outbak, 1);
	    close (h_out);
	    close (h_outbak);
	  }

	  if (token == EOL)
	    done = 1;
	  else
	  {
	    if (token == SEMICOLON)
	      f_in = f_out = 0;
	    s = u;
	  }
	  break;
	case UNMATCHED_QUOTE:
	  result = emiterror ("Unmatched quote character.", 0);
	  errno = EINVAL;
	  goto leave;
	default:
	  result = emiterror ("I cannot grok this.", 0);
	  errno = EINVAL;
	  goto leave;
	}
      } while (again);
    }
  leave:
    if (rm_in)
      remove (f_in);
    if (rm_out)
      remove (f_out);

    return result;
  }
}
#endif

#if NEW_KPSE







#endif
#endif

