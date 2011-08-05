/* stackenv.c: routines that help to mimic shell scripts.

Copyright (C) 1997 Fabrice POPINEAU.

Adapted to DJGPP by Eli Zaretskii.

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


#include <kpathsea/config.h>
#ifdef __MINGW32__
#define fcloseall _fcloseall
#define flushall _flushall
#endif
#include <kpathsea/kpathsea.h>
#include "stackenv.h"

const_string mktex_version_string = "2.0";

extern char *output;
extern char tmpdir[];
extern char *progname;

/* The global stack. */
static mod_env stack_env[256];
static int index_env = 0;

void mt_exit(int);

static FILE *fnul = NULL;

pfnOutputAndCleanup output_and_cleanup_function;

#ifndef _WIN32
#include <unistd.h>

static void flushall (void)
{
  /* Simulate `flushall' by only flushing standard streams.
     This should be enough, since the non-standard are going
     to be closed in a moment, and will be flushed then.  */
  fflush (stdout);
  fsync (fileno (stdout));
  fflush (stderr);
  fsync (fileno (stderr));
}
#endif

#ifndef _WIN32
#ifdef __DJGPP__

/* DJGPP-specific emulation of functions which aren't in the library
   and aren't ANSI/POSIX.  */

#include <errno.h>
#include <libc/file.h>

static void fcloseall_func (FILE *f)
{
  fflush (f);
  if (fileno (f) > 2)
    fclose(f);
  else
    fsync (fileno (f));
}

void fcloseall (void)
{
  _fwalk (fcloseall_func);
}

#else  /* not __DJGPP__ */

/* FIXME: is this enough on Unix?  */
static void fcloseall (void)
{
  flushall ();
}

#endif /* not __DJGPP__ */
#endif /* not _WIN32 */

void
oops(const char *message, ...)
{
        va_list args;

        va_start(args, message);
        vfprintf(stderr, message, args);
        va_end(args);
        fputc('\n', stderr);
        mt_exit(1);
}

/* pushd */
void pushd(char *p)
{
  if (index_env >= sizeof(stack_env)/sizeof(stack_env[0])) {
    fprintf(stderr, "%s: stack overflow in pushd\n", progname);
    mt_exit(1);
  }
  if ((stack_env[index_env].data.path = malloc(MAXPATHLEN)) == NULL
      || getcwd(stack_env[index_env].data.path, MAXPATHLEN) == NULL) {
    fprintf(stderr, "pushd error!\n");
    mt_exit(1);
  }
  stack_env[index_env].op = CHDIR;
  if (KPSE_DEBUG_P(MKTEX_FINE_DEBUG)) {
    fprintf(stderr, "At %d, pushing from %s to %s\n", index_env, 
	    stack_env[index_env].data.path, p);
  }

  index_env++;
  if (chdir(p) == -1) {
    perror(p);
    mt_exit(1);
  }
}
/* popd */
void popd(void)
{
  if (index_env == 0)
    return;
  index_env--;
  if (KPSE_DEBUG_P(MKTEX_FINE_DEBUG)) {
    fprintf(stderr, "At %d, popping %s\n", index_env, stack_env[index_env].data.path);
  }
  assert(stack_env[index_env].op == CHDIR);
  if (chdir(stack_env[index_env].data.path) == -1) {
    perror(stack_env[index_env].data.path);
    mt_exit(1);
  }
  free(stack_env[index_env].data.path);
  return;
}

/* random access to directories' stack  */
char *peek_dir(int n)
{
  int i;
  int level = n;

  /* The stack holds both directories pushed by `pushd' and file
     handles pushed by `push_fd'.  When they say "peek_dir(n)", they
     mean the n'th DIRECTORY entry, not the n'th entry counting from
     the stack bottom.

     For example, if the stack looks like this:

		  idx      type

		   0	  handles
		   1	  handles
		   2	  directory
		   3	  handles    <--- top of the stack
		   4      <empty>    <--- index_env

     then "peek_dir(0)" should return the entry with the index 2, not
     the one with the index 0.

     The following loop looks for the index i of a stack slot which
     holds the n'th directory pushed on to the stack.  */

  do {
    for (i = 0; i < index_env && stack_env[i].op != CHDIR; i++)
      ;
  } while (i < index_env && n--);

  if (i < index_env)
    return stack_env[i].data.path;
  else {
    fprintf (stderr, "%s: attempt to peek at invalid stack level %d\n",
	     progname, level);
    mt_exit(1);
  }
}

/* redirect */
void push_fd(int newfd[3])
{
  int i;

  if (KPSE_DEBUG_P(MKTEX_FINE_DEBUG)) {
    fprintf(stderr, "At %d, pushing fds %d %d %d\n", index_env, newfd[0], 
	    newfd[1], newfd[2]);
  }
  flushall();
  if (index_env >= sizeof(stack_env)/sizeof(stack_env[0])) {
    fprintf(stderr, "%s: stack overflow in push_fd\n", progname);
    mt_exit(1);
  }
  stack_env[index_env].op = REDIRECT;
  for(i = 0; i < 3; i++) {
    if (i == newfd[i])
      stack_env[index_env].data.oldfd[i] = i;
    else {
      if ((stack_env[index_env].data.oldfd[i] = dup(i)) == -1) {
	perror("push_fd: dup");
	mt_exit(1);
      }
      if (dup2(newfd[i], i) == -1) {
	perror("push_fd : dup2");
	mt_exit(1);
      }
    }
  }
  index_env++;
}

void pop_fd(void)
{
  int i;
  index_env--;
  assert(stack_env[index_env].op == REDIRECT);
  if (KPSE_DEBUG_P(MKTEX_FINE_DEBUG)) {
    fprintf(stderr, "At %d; popping fds %d %d %d\n", 
	    index_env,
	    stack_env[index_env].data.oldfd[0],
	    stack_env[index_env].data.oldfd[1],
	    stack_env[index_env].data.oldfd[2]);
  } 
  flushall();
  for(i = 0; i < 3; i++)
    if (i != stack_env[index_env].data.oldfd[i]) {
      if (fnul && i != fileno(fnul)) close(i);
      if (dup2(stack_env[index_env].data.oldfd[i], i) == -1) {
	perror("pop_fd : dup2");
	mt_exit(1);
      }
      if (fnul && stack_env[index_env].data.oldfd[i] != fileno(fnul))
	close(stack_env[index_env].data.oldfd[i]);
    }
}

/* popenv */
void popenv(void)
{
  if (index_env <= 0) {
    if (KPSE_DEBUG_P(MKTEX_FINE_DEBUG))
      fprintf(stderr, "Trying to popenv() from empty stack!\n");
    return;
  }

  switch(stack_env[index_env-1].op) {
  case CHDIR:
    popd();
    break;
  case REDIRECT:
    pop_fd();
    break;
  default:
    fprintf(stderr, "popenv : unknown op %d.\n", stack_env[index_env-1].op);
    break;
  }
}

#ifdef _WIN32
int sigint_handler(DWORD dwCtrlType)
{
  /* Fix me : there is a problem if a system() command is running.
     We should wait for the son process to be interrupted.
     Only way I can think of to do that : rewrite system() based on
     spawn() with parsing of the command line and set a global pid
     Next cwait(pid) in the HandlerRoutine. 
     */

  /* This is not that good, but else we would need to wait for 
     the child processes to finish ! */
  Sleep(250);

  mt_exit(3);
  return FALSE;			/* return value obligatory */
}
#else
void sigint_handler (int sig)
{
  mt_exit(3);
}
#endif

void mt_exit(int code)
{
  /* rétablir les 0, 1 et 2 d'origine avant de tenter quoi que ce soit ! */
  fcloseall();

  /* unstack all env from main) */
  for( ; index_env > 0; popenv());

  /* output the result and rm any unneeded file */
  if (output_and_cleanup_function != 0)
    (*output_and_cleanup_function)(code);

  exit(code);
}

#if 0 /* unused */
static void
dump_stack(void)
{
  int i;
  for (i = 0; i < index_env; i++)
    switch (stack_env[i].op) {
    case CHDIR:
      fprintf(stderr, "Env %d : chdir %s\n", i, stack_env[i].data.path);
      break;
    case REDIRECT:
      fprintf(stderr, "Env %d : redirect %d %d %d\n", i, 
	      stack_env[i].data.oldfd[0],
	      stack_env[i].data.oldfd[1],
	      stack_env[i].data.oldfd[2]);
      break;
    }

}
#endif

/*
  This is used by mktex{mf,tfm,pk}
  if the argument is true, stdout is redirected to fnul
  else to stderr.
  */
void start_redirection(int discard_stdout)
{
  int newfd[3];
  if (fnul == NULL)
#ifdef _WIN32
  fnul = fopen("nul", "r");	/* fopen /dev/null */
#else
  fnul = fopen("/dev/null", "r");
#endif
  
  newfd[0] = fileno(fnul);
  newfd[1] = (discard_stdout ? fileno(fnul) : fileno(stderr));
  newfd[2] = fileno(stderr);
  
  push_fd(newfd);
}
