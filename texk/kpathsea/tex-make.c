/* tex-make.c: run external programs to make TeX-related files.

   Copyright 1993, 1994, 1995, 1996, 1997, 2008-2020 Karl Berry.
   Copyright 1997, 1998, 2001-05 Olaf Weber.

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

#include <kpathsea/c-fopen.h>
#include <kpathsea/c-pathch.h>
#include <kpathsea/db.h>
#include <kpathsea/fn.h>
#include <kpathsea/magstep.h>
#include <kpathsea/readable.h>
#include <kpathsea/tex-make.h>
#include <kpathsea/variable.h>

#if !defined (AMIGA) && !(defined (MSDOS) && !defined(__DJGPP__)) && !defined (WIN32)
#include <sys/wait.h>
#endif


/* We set the envvar MAKETEX_MAG, which is part of the default spec for
   MakeTeXPK above, based on KPATHSEA_DPI and MAKETEX_BASE_DPI.  */

static void
set_maketex_mag (kpathsea kpse)
{
  char q[MAX_INT_LENGTH * 3 + 3];
  int m;
  string dpi_str = getenv ("KPATHSEA_DPI");
  string bdpi_str = getenv ("MAKETEX_BASE_DPI");
  unsigned dpi = dpi_str ? atoi (dpi_str) : 0;
  unsigned bdpi = bdpi_str ? atoi (bdpi_str) : 0;

  /* If the environment variables aren't set, it's a bug.  */
  assert (dpi != 0 && bdpi != 0);

  /* Fix up for roundoff error.  Hopefully the driver has already fixed
     up DPI, but may as well be safe, and also get the magstep number.  */
  (void) kpathsea_magstep_fix (kpse, dpi, bdpi, &m);

  if (m == 0) {
      if (bdpi <= 4000) {
          sprintf(q, "%u+%u/%u", dpi / bdpi, dpi % bdpi, bdpi);
      } else {
          unsigned f = bdpi/4000;
          unsigned r = bdpi%4000;

          if (f > 1) {
              if (r > 0) {
                  sprintf(q, "%u+%u/(%u*%u+%u)",
                          dpi/bdpi, dpi%bdpi, f, (bdpi - r)/f, r);
              } else {
                  sprintf(q, "%u+%u/(%u*%u)", dpi/bdpi, dpi%bdpi, f, bdpi/f);
              }
          } else {
              sprintf(q, "%u+%u/(4000+%u)", dpi/bdpi, dpi%bdpi, r);
          }
      }
  } else {
      /* m is encoded with LSB being a ``half'' bit (see magstep.h).  Are
         we making an assumption here about two's complement?  Probably.
         In any case, if m is negative, we have to put in the sign
         explicitly, since m/2==0 if m==-1.  */
      const_string sign = "";
      if (m < 0) {
          m *= -1;
          sign = "-";
      }
      sprintf(q, "magstep\\(%s%d.%d\\)", sign, m / 2, (m & 1) * 5);
  }
  kpathsea_xputenv (kpse, "MAKETEX_MAG", q);
}

/* This mktex... program was disabled, or the script failed.  If this
   was a font creation (according to FORMAT), append CMD
   to a file missfont.log in the current directory.  */

static void
misstex (kpathsea kpse, kpse_file_format_type format,  string *args)
{
  string *s;

  /* If we weren't trying to make a font, do nothing.  Maybe should
     allow people to specify what they want recorded?  */
  if (format != kpse_gf_format
      && format != kpse_pk_format
      && format != kpse_any_glyph_format
      && format != kpse_tfm_format
      && format != kpse_vf_format)
    return;

  /* If this is the first time, have to open the log file.  But don't
     bother logging anything if they were discarding errors.  */
  if (!kpse->missfont && !kpse->make_tex_discard_errors) {
    const_string missfont_name = kpathsea_var_value (kpse, "MISSFONT_LOG");
    if (!missfont_name || *missfont_name == '1') {
      missfont_name = "missfont.log"; /* take default name */
    } else if (missfont_name
               && (*missfont_name == 0 || *missfont_name == '0')) {
      missfont_name = NULL; /* user requested no missfont.log */
    } /* else use user's name */

    kpse->missfont
      = missfont_name ? fopen (missfont_name, FOPEN_A_MODE) : NULL;
    if (!kpse->missfont && kpathsea_var_value (kpse, "TEXMFOUTPUT")) {
      missfont_name = concat3 (kpathsea_var_value (kpse, "TEXMFOUTPUT"),
                               DIR_SEP_STRING, missfont_name);
      kpse->missfont = fopen (missfont_name, FOPEN_A_MODE);
    }

    if (kpse->missfont)
      fprintf (stderr, "kpathsea: Appending font creation commands to %s.\n",
               missfont_name);
  }

  /* Write the command if we have a log file.  */
  if (kpse->missfont) {
    fputs (args[0], kpse->missfont);
    for (s = &args[1]; *s != NULL; s++) {
      putc(' ', kpse->missfont);
      fputs (*s, kpse->missfont);
    }
    putc ('\n', kpse->missfont);
  }
}


/* Assume the script outputs the filename it creates (and nothing
   else) on standard output; hence, we run the script with `popen'.  */

static string
maketex (kpathsea kpse, kpse_file_format_type format, string* args)
{
  /* New implementation, use fork/exec pair instead of popen, since
   * the latter is virtually impossible to make safe.
   */
  unsigned len;
  string *s;
  string ret = NULL;
  string fn;
#if defined(WIN32)
  char   fullbin[256], *wrp;

  wrp = kpathsea_var_value(kpse, "SELFAUTOLOC");
  if(wrp == NULL) {
     fprintf(stderr, "I cannot get SELFAUTOLOC\n");
     exit(100);
  }

  strcpy(fullbin, wrp);
  free(wrp);
  for(wrp=fullbin; *wrp; wrp++) {
     if(*wrp == '/') *wrp = '\\';
  }
  strcat(fullbin, "\\");
  strcat(fullbin, args[0]);
#endif
  if (!kpse->make_tex_discard_errors) {
    fprintf (stderr, "\nkpathsea: Running");
    for (s = &args[0]; *s != NULL; s++)
      fprintf (stderr, " %s", *s);
    fputc('\n', stderr);
  }

#if defined (AMIGA)
  /* Amiga has a different interface. */
  {
    string cmd;
    string newcmd;
    cmd = xstrdup(args[0]);
    for (s = &args[1];  *s != NULL; s++) {
      newcmd = concat(cmd, *s);
      free (cmd);
      cmd = newcmd;
    }
    ret = system(cmd) == 0 ? getenv ("LAST_FONT_CREATED"): NULL;
    free (cmd);
  }
#elif defined (MSDOS) && !defined(__DJGPP__)
#error Implement new MSDOS mktex call interface here
#else /* WIN32 or Unix */
  {
#if defined (WIN32)
    /* spawnvp(_P_NOWAIT, ...) and pipe --ak 2002/12/15 */

    unsigned long nexitcode = STILL_ACTIVE;
    HANDLE hchild;
    int hstdout, childpipe[2];
    int hstderr = -1;
    FILE *Hnul = NULL;

    fn = NULL;

    if(_pipe(childpipe, 1024, O_TEXT | _O_NOINHERIT) == -1) {
      perror("kpathsea: pipe()");
      goto labeldone;
    }

    hstdout = _dup(fileno(stdout));
    if(_dup2(childpipe[1], fileno(stdout)) != 0) {
      close(hstdout);
      close(childpipe[0]);
      close(childpipe[1]);
      goto labeldone;
    }

    close(childpipe[1]);

    if(kpse->make_tex_discard_errors) {
      Hnul = fopen("nul", "w");
      if(!Hnul) {
        perror("kpathsea: fopen(\"nul\")");
      }
      else {
        hstderr = _dup(fileno(stderr));
        _dup2(fileno(Hnul), fileno(stderr));
      }
    }
    fprintf(stderr, "\nThe command name is %s\n", fullbin);
    hchild = (HANDLE)_spawnvp(_P_NOWAIT, fullbin, (const char * const *) args);

    _dup2(hstdout, fileno(stdout));
    close(hstdout);

    if(hchild == (HANDLE)(-1)) {
      close(childpipe[0]);
      goto labeldone;
    }

    if(hchild) {
      char buf[1024+1];
      int num;

      fn = xstrdup("");
      while(nexitcode == STILL_ACTIVE) {
        num = read(childpipe[0], buf, sizeof(buf)-1);
        if(num) {
          string newfn;
          buf[num] = '\0';
          newfn = concat(fn, buf);
          free(fn);
          fn = newfn;
        }
        if(!GetExitCodeProcess(hchild, &nexitcode)) {
          fn = NULL;
          close(childpipe[0]);
          goto labeldone;
        }
      }
      close(childpipe[0]);
    }

 labeldone:
    if(kpse->make_tex_discard_errors && Hnul) {
       _dup2(hstderr, fileno(stderr));
       close(hstderr);
       fclose(Hnul);
    }
#else /* !WIN32 */
    /* Standard input for the child.  Set to /dev/null */
    int childin;
    /* Standard output for the child, what we're interested in. */
    int childout[2];
    /* Standard error for the child, same as parent or /dev/null */
    int childerr;
    /* Child pid. */
    pid_t childpid;

    /* Open the channels that the child will use. */
    /* A fairly horrible uses of gotos for here for the error case. */
    if ((childin = open("/dev/null", O_RDONLY)) < 0) {
      perror("kpathsea: open(\"/dev/null\", O_RDONLY)");
      goto error_childin;
    }
    if (pipe(childout) < 0) {
      perror("kpathsea: pipe()");
      goto error_childout;
    }
    if ((childerr = open("/dev/null", O_WRONLY)) < 0) {
      perror("kpathsea: open(\"/dev/null\", O_WRONLY)");
      goto error_childerr;
    }
    if ((childpid = fork()) < 0) {
      perror("kpathsea: fork()");
      close(childerr);
     error_childerr:
      close(childout[0]);
      close(childout[1]);
     error_childout:
      close(childin);
     error_childin:
      fn = NULL;
    } else if (childpid == 0) {
      /* Child
       *
       * We can use vfork, provided we're careful about what we
       * do here: do not return from this function, do not modify
       * variables, call _exit if there is a problem.
       *
       * Complete setting up the file descriptors.
       * We use dup(2) so the order in which we do this matters.
       */
      close(childout[0]);
      /* stdin -- the child will not receive input from this */
      if (childin != 0) {
        close(0);
        if (dup(childin) != 0) {
          perror("kpathsea: dup(2) failed for stdin");
          close(childin);
          _exit(1);
        }
        close(childin);
      }
      /* stdout -- the output of the child's action */
      if (childout[1] != 1) {
        close(1);
        if (dup(childout[1]) != 1) {
          perror("kpathsea: dup(2) failed for stdout");
          close(childout[1]);
          _exit(1);
        }
        close(childout[1]);
      }
      /* stderr -- use /dev/null if we discard errors */
      if (childerr != 2) {
        if (kpse->make_tex_discard_errors) {
          close(2);
          if (dup(childerr) != 2) {
            perror("kpathsea: dup(2) failed for stderr");
            close(childerr);
            _exit(1);
          }
        }
        close(childerr);
      }
      /* FIXME: We could/should close all other file descriptors as well. */
      /* exec -- on failure a call of _exit(2) it is the only option */
      if (execvp(args[0], args))
        perror(args[0]);
      _exit(1);
    } else {
      /* Parent */
      char buf[1024+1];
      int num;

      /* Clean up child file descriptors that we won't use anyway. */
      close(childin);
      close(childout[1]);
      close(childerr);
      /* Get stdout of child from the pipe. */
      fn = xstrdup("");
      while ((num = read(childout[0],buf,sizeof(buf)-1)) != 0) {
        if (num == -1) {
          if (errno != EINTR) {
            perror("kpathsea: read()");
            break;
          }
        } else {
          string newfn;
          buf[num] = '\0';
          newfn = concat(fn, buf);
          free(fn);
          fn = newfn;
        }
      }
      /* End of file on pipe, child should have exited at this point. */
      close(childout[0]);
      /* We don't really care about the exit status at this point. */
      wait(NULL);
    }
#endif /* !WIN32 */

    if (fn) {
      len = strlen(fn);

      /* Remove trailing newlines and returns.  */
      while (len && (fn[len - 1] == '\n' || fn[len - 1] == '\r')) {
        fn[len - 1] = '\0';
        len--;
      }

      ret = len == 0 ? NULL : kpathsea_readable_file (kpse, fn);
      if (!ret && len > 1) {
        WARNING2 ("kpathsea: %s output `%s' instead of a filename",
                  args[0], fn);
      }

      /* Free the name if we're not returning it.  */
      if (fn != ret)
        free (fn);
    }
  }
#endif /* WIN32 or Unix */

  if (ret == NULL)
      misstex (kpse, format, args);
  else
      kpathsea_db_insert (kpse, ret);

  return ret;
}



/* Create BASE in FORMAT and return the generated filename, or
   return NULL.  We used to emit warnings for names we declined to pass
   on to the scripts, but such names are common with system fonts, so
   now we are silent (just returning NULL).  That is arguably better
   behavior anyway.  Presumably the caller always reports "font not
   found" anyway.  */

string
kpathsea_make_tex (kpathsea kpse, kpse_file_format_type format,
                   const_string base)
{
  kpse_format_info_type spec; /* some compilers lack struct initialization */
  string ret = NULL;

  spec = kpse->format_info[format];
  if (!spec.type) { /* Not initialized yet? */
    kpathsea_init_format (kpse, format);
    spec = kpse->format_info[format];
  }

  if (spec.program && spec.program_enabled_p) {
    /* See the documentation for the envvars we're dealing with here.  */
    /* Number of arguments is spec.argc + 1, plus the trailing NULL. */
    string *args = XTALLOC (spec.argc + 2, string);
    /* Helpers */
    int argnum;
    int i;

    /* Check whether the name we were given is likely to be a problem.
       All could be fixed in the scripts and/or invocation, but in
       practice our names are simple, so let's err on the side of strictness:
       - may not start with a hyphen
       - allowed are: alphanumeric, underscore, hyphen, period, plus
       - also allowed: DIRSEP, as we can be fed that when creating pk fonts
       
       For example, system fonts are likely to contain spaces, and
       (for filename lookups) be enclosed in square brackets.  We don't
       want to try calling our mktex* scripts on those.
    */
    if (base[0] == '-' /* || IS_DIR_SEP(base[0])  */) {
      return NULL;
    }
    for (i = 0; base[i]; i++) {
      if (!ISALNUM(base[i])
          && base[i] != '-'
          && base[i] != '+'
          && base[i] != '_'
          && base[i] != '.'
          && !IS_DIR_SEP(base[i]))
      {
        return NULL;
      }
    }

    if (format == kpse_gf_format
        || format == kpse_pk_format
        || format == kpse_any_glyph_format)
      set_maketex_mag (kpse);

    /* Here's an awful kludge: if the mode is `/', mktexpk recognizes
       it as a special case.  `kpse_prog_init' sets it to this in the
       first place when no mode is otherwise specified; this is so
       when the user defines a resolution, they don't also have to
       specify a mode; instead, mktexpk's guesses will take over.
       They use / for the value because then when it is expanded as
       part of the PKFONTS et al. path values, we'll wind up searching
       all the pk directories.  We put $MAKETEX_MODE in the path
       values in the first place so that sites with two different
       devices with the same resolution can find the right fonts; but
       such sites are uncommon, so they shouldn't make things harder
       for everyone else.  */
    for (argnum = 0; argnum < spec.argc; argnum++) {
        args[argnum] = kpathsea_var_expand (kpse, spec.argv[argnum]);
    }
    args[argnum++] = xstrdup(base);
    args[argnum] = NULL;

    ret = maketex (kpse, format, args);

    for (argnum = 0; args[argnum] != NULL; argnum++)
      free (args[argnum]);
    free (args);
  }

  return ret;
}

#if defined (KPSE_COMPAT_API)
string
kpse_make_tex (kpse_file_format_type format,  const_string base)
{
  return kpathsea_make_tex (kpse_def, format, base);
}
#endif


#ifdef TEST

void
test_make_tex (kpathsea kpse, kpse_file_format_type fmt, const_string base)
{
  string answer;

  printf ("\nAttempting %s in format %d:\n", base, fmt);

  answer = kpathsea_make_tex (kpse, fmt, base);
  puts (answer ? answer : "(nil)");
}


int
main (int argc, char **argv)
{
  kpathsea kpse = xcalloc(1, sizeof(kpathsea_instance));
  kpathsea_set_program_name(kpse, argv[0], NULL);
  kpathsea_xputenv (kpse, "KPATHSEA_DPI", "781"); /* call mktexpk */
  kpathsea_xputenv (kpse,"MAKETEX_BASE_DPI", "300"); /* call mktexpk */
  kpathsea_set_program_enabled(kpse, kpse_pk_format, 1, kpse_src_env);
  test_make_tex (kpse, kpse_pk_format, "cmr10");

  /* Fail with mktextfm.  */
  kpathsea_set_program_enabled(kpse, kpse_tfm_format, 1, kpse_src_env);
  test_make_tex (kpse, kpse_tfm_format, "foozler99");

  /* Call something disabled.  */
  test_make_tex (kpse, kpse_bst_format, "no-way");

  return 0;
}

#endif /* TEST */


/*
Local variables:
standalone-compile-command: "gcc -g -I. -I.. -DTEST tex-make.c kpathsea.a"
End:
*/
