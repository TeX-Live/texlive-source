/* progname.c: the executable name we were invoked as; general initialization.

   Copyright 1994, 1996, 1997, 2008-2013, 2016 Karl Berry.
   Copyright 1998-2005 Olaf Weber.

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
#include <kpathsea/absolute.h>
#include <kpathsea/c-pathch.h>
#include <kpathsea/c-pathmx.h>
#include <kpathsea/c-stat.h>
#include <kpathsea/pathsearch.h>
/* For kpse_reset_progname */
#include <kpathsea/tex-file.h>


#if defined(__i386_pc_gnu__)
#ifndef _S_ISUID
#define _S_ISUID    04000  /* Set user ID on execution.  */
#endif
#ifndef _S_ISGID
#define _S_ISGID    02000  /* Set group ID on execution.  */
#endif
#ifndef _S_ISVTX
#define _S_ISVTX    01000  /* Save swapped text after use (sticky).  */
#endif
#ifndef _S_IREAD
#define _S_IREAD    0400   /* Read by owner.  */
#endif
#ifndef _S_IWRITE
#define _S_IWRITE   0200   /* Write by owner.  */
#endif
#ifndef _S_IEXEC
#define _S_IEXEC    0100   /* Execute by owner.  */
#endif
#endif

/* NeXT does not define the standard macros, but has the equivalent.
   WIN32 doesn't define them either, and doesn't have them.
   From: Gregor Hoffleit <flight@mathi.uni-heidelberg.de>.  */
#ifndef S_IXUSR
#ifdef WIN32
#define S_IXUSR 0
#define S_IXGRP 0
#define S_IXOTH 0
#else /* not WIN32 */
#define S_IXUSR 0100
#endif /* not WIN32 */
#endif /* not S_IXUSR */
#ifndef S_IXGRP
#define S_IXGRP 0010
#endif
#ifndef S_IXOTH
#define S_IXOTH 0001
#endif


#ifndef WIN32
/* From a standalone program `ll' to expand symlinks written by Kimbo Mundy.
   Don't bother to compile if we don't have symlinks; thus we can assume
   / as the separator.  Also don't try to use basename, etc., or
   handle arbitrary filename length.  Mixed case function names because
   that's what kimbo liked.  */

#ifdef S_ISLNK

#undef BSIZE
#define BSIZE 2048 /* sorry */


/* Read link FN into SYM.  */

static void
ReadSymLink (char *fn, char *sym)
{
  register int n = readlink (fn, sym, BSIZE);
  if (n < 0) {
    perror (fn);
    exit (1);
  }
  sym[n] = 0;
}


/* Strip first component from S, and also return it in a static buffer.  */

static char *
StripFirst (register char *s)
{
  static char buf[BSIZE];
  register char *s1;

  /* Find the end of the first path element */
  for (s1 = s; *s1 && (*s1 != '/' || s1 == s); s1++)
    ;

  /* Copy it into buf and null-terminate it. */
  strncpy (buf, s, s1 - s);
  buf[s1 - s] = 0;

  /* Skip over the leading / (if any) */
  if (*s1 == '/')
    ++s1;

  /* Squeeze out the element */
  while ((*s++ = *s1++) != 0)
    ;

  return buf;
}


/* Strip last component from S, and also return it in a static buffer.  */

static char *
StripLast (register char *s)
{
  static char buf[BSIZE];
  register char *s1;

  for (s1 = s + strlen (s); s1 > s && *s1 != '/'; s1--)
    ;
  strcpy (buf, s1 + (*s1 == '/'));
  *s1 = 0;

  return buf;
}


/* Copy first path element from B to A, removing it from B.  */

static void
CopyFirst (register char *a, char *b)
{
  register int length = strlen (a);

   if (length > 0 && a[length - 1] != '/') {
   a[length] = '/';
    a[length + 1] = 0;
  }
  strcat (a, StripFirst (b));
}

/* Returns NULL on error.  Prints intermediate results if global
   `ll_verbose' is nonzero.  */

#define EX(s)           (strlen (s) && strcmp (s, "/") ? "/" : "")
#define EXPOS           EX(post)
#define EXPRE           EX(pre)

static char *
expand_symlinks (kpathsea kpse, char *s)
{
  static char pre[BSIZE];       /* return value */
  char post[BSIZE], sym[BSIZE], tmp[BSIZE], before[BSIZE];
  char *cp;
  char a;
  struct stat st;
  int done;

  /* Check for symlink loops.  It's difficult to check for all the
     possibilities ourselves, so let the kernel do it.  And make it
     conditional so that people can see where the infinite loop is
     being caused (see engtools#1536).  */
  /* There used to be a test for a variable |ll_loop| here, but
     it was initialized to zero and never updated */
  if (0) {
    FILE *f = fopen (s, "r");
    if (!f && errno == ELOOP) {
      /* Not worried about other errors, we'll get to them in due course.  */
      perror (s);
      return NULL;
    }
    if (f) fclose (f);
  }

  strcpy (post, s);
  strcpy (pre, "");

  while (strlen (post) != 0) {
    CopyFirst (pre, post);

    if (lstat (pre, &st) != 0) {
      fprintf (stderr, "lstat(%s) failed ...\n", pre);
      perror (pre);
      return NULL;
    }

    if (S_ISLNK (st.st_mode)) {
      ReadSymLink (pre, sym);

      if (!strncmp (sym, "/", 1)) {
        if (kpse->ll_verbose)
          printf ("[%s]%s%s -> [%s]%s%s\n", pre, EXPOS, post, sym, EXPOS,post);
        strcpy (pre, "");

      } else {
        a = pre[0];     /* handle links through the root */
        strcpy (tmp, StripLast (pre));
        if (!strlen (pre) && a == '/')
          strcpy (pre, "/");

        if (kpse->ll_verbose) {
          sprintf (before, "%s%s[%s]%s%s", pre, EXPRE, tmp, EXPOS, post);
          printf ("%s -> %s%s[%s]%s%s\n", before, pre, EXPRE, sym, EXPOS,post);
        }

        /* Strip "../" path elements from the front of sym; print
           new result if there were any such elements.  */
        done = 0;
        a = pre[0];     /* handle links through the root */
        while (!strncmp (sym, "..", 2)
               && (sym[2] == 0 || sym[2] == '/')
               && strlen (pre) != 0
               && strcmp (pre, ".")
               && strcmp (pre, "..")
               && (strlen (pre) < 3
                   || strcmp (pre + strlen (pre) - 3, "/.."))) {
          done = 1;
          StripFirst (sym);
          StripLast (pre);
        }

        if (done && kpse->ll_verbose) {
          for (cp = before; *cp;)
            *cp++ = ' ';
          if (strlen (sym))
            printf ("%s == %s%s%s%s%s\n", before, pre, EXPRE, sym, EXPOS,post);
          else
            printf ("%s == %s%s%s\n", before, pre, EXPOS, post);
        }
        if (!strlen (pre) && a == '/')
          strcpy (pre, "/");
      }

      if (strlen (post) != 0 && strlen (sym) != 0)
        strcat (sym, "/");

      strcat (sym, post);
      strcpy (post, sym);
    }
  }

  return pre;
}
#else /* not S_ISLNK */
#define expand_symlinks(k,s) (s)
#endif /* not S_ISLNK */

/* Remove .'s and ..'s in DIR, to avoid problems with relative symlinks
   as the program name, etc.  This does not canonicalize symlinks.  */

static string
remove_dots (kpathsea kpse, string dir)
{
#ifdef AMIGA
  return dir;
#else
  string c;
  unsigned len;
  string ret = NULL;

  for (c = kpathsea_filename_component (kpse, dir); c;
       c = kpathsea_filename_component (kpse, NULL)) {
    if (STREQ (c, ".")) {
      /* If leading ., replace with cwd.  Else ignore.  */
      if (!ret) {
        ret = xgetcwd ();
      }

    } else if (STREQ (c, "..")) {
      /* If leading .., start with xdirname (cwd).  Else remove last
         component from ret, if any.  */
      if (!ret) {
        string dot = xgetcwd ();
        ret = xdirname (dot);
        free (dot);
      } else {
        unsigned last;
        string p = NAME_BEGINS_WITH_DEVICE (ret) ? ret + 2 : ret;
        for (last = strlen (p); last > 0; last--) {
          if (IS_DIR_SEP_CH (p[last - 1])) {
            /* If we have `/../', that's the same as `/'.  */
            p[(last > 1 ? last - 1 : 1)] = 0;
            break;
          }
        }
      }

    } else {
      /* Not . or ..; just append.  Include a directory separator unless
         our string already ends with one.  This also changes all directory
         separators into the canonical DIR_SEP_STRING.  */
      if (!ret) {
        ret = concat (NAME_BEGINS_WITH_DEVICE (c) ? "" : DIR_SEP_STRING, c);
      } else {
        string temp = ret;
        len = strlen (ret);
        ret = concat3 (ret, ret[len - 1] == DIR_SEP ? "" : DIR_SEP_STRING, c);
        free (temp);
      }
    }
  }
  assert (ret);

  /* Remove a trailing /, just in case it snuck in.  */
  len = strlen (ret);
  if (len > 0 && ret[len - 1] == DIR_SEP) {
    ret[len - 1] = 0;
  }

  return ret;
#endif /* not AMIGA */
}

/* Return directory ARGV0 comes from.  Check PATH if ARGV0 is not
   absolute.  */

string
kpathsea_selfdir (kpathsea kpse, const_string argv0)
{
  string self = NULL;
  string name;
  string ret;

  if (kpathsea_absolute_p (kpse, argv0, true)) {
    self = xstrdup (argv0);
  } else {
#ifdef AMIGA
#include <dos.h>
#include <proto/dos.h>
#include <proto/exec.h>
    BPTR lock;
    struct DosLibrary *DOSBase
      = (struct DosLibrary *) OpenLibrary ("dos.library", 0L);
    assert (DOSBase);

    self = xmalloc (BUFSIZ);
    lock = findpath (argv0);
    if (lock != ((BPTR) -1)) {
      if (getpath (lock, self) == -1) {
        *self = '\0';
      } else {
        strcat (self,DIR_SEP_STRING);
        strcat (self,argv0);
      }
      UnLock (lock);
    }
    CloseLibrary((struct Library *) DOSBase);
#else /* not AMIGA */
    const_string elt;
    struct stat s;

    /* Have to check PATH.  But don't call kpse_path_search since we don't
       want to search any ls-R's or do anything special with //'s.  */
    for (elt = kpathsea_path_element (kpse, getenv ("PATH")); !self && elt;
         elt = kpathsea_path_element (kpse, NULL)) {
      /* UNIX tradition interprets the empty path element as "." */
      if (*elt == 0) elt = ".";

      name = concat3 (elt, DIR_SEP_STRING, argv0);

      /* In order to do this perfectly, we'd have to check the owner bits only
         if we are the file owner, and the group bits only if we belong
         to the file group.  That's a lot of work, though, and it's not
         likely that kpathsea will ever be used with a program that's
         only executable by some classes and not others.  See the
         `file_status' function in execute_cmd.c in bash for what's
         necessary if we were to do it right.  */
      if (stat (name, &s) == 0 && s.st_mode & (S_IXUSR|S_IXGRP|S_IXOTH)
                               /* Do not stop at directories. */
                               && !S_ISDIR(s.st_mode))
        self = name;
      else
        free (name);
    }
#endif /* not AMIGA */
  }

  /* If argv0 is somehow dir/exename, `self' will still be NULL.  */
  if (!self)
    self = concat3 (".", DIR_SEP_STRING, argv0);

  name = remove_dots (kpse, expand_symlinks (kpse, self));

#ifndef AMIGA
  free (self);
#endif

  ret = xdirname (name);

  free (name);

  return ret;
}

#if defined (KPSE_COMPAT_API)
string
kpse_selfdir (const_string argv0)
{
    return kpathsea_selfdir (kpse_def, argv0);
}
#endif

#endif /* not WIN32 */

#if defined(WIN32) || defined(__CYGWIN__)

/* Create a list of executable suffixes of files not to be written.  */
#define EXE_SUFFIXES ".com;.exe;.bat;.cmd;.vbs;.vbe;.js;.jse;.wsf;.wsh;.ws;.tcl;.py;.pyw"

static void
mk_suffixlist (kpathsea kpse)
{
    char **p;
    char *q, *r, *v;
    int  n;

#if defined(__CYGWIN__)
    v = xstrdup (EXE_SUFFIXES);
#else
    v = getenv ("PATHEXT");
    if (v) /* strlwr() exists also in MingW */
      v = strlwr (xstrdup (v));
    else
      v = xstrdup (EXE_SUFFIXES);
#endif

    q = v;
    n = 0;

    while ((r = strchr (q, ';')) != NULL) {
      n++;
      r++;
      q = r;
    }
    if (*q)
      n++;
    kpse->suffixlist = (char **) xmalloc ((n + 2) * sizeof (char *));
    p = kpse->suffixlist;
    *p = xstrdup (".dll");
    p++;
    q = v;
    while ((r = strchr (q, ';')) != NULL) {
      *r = '\0';
      *p = xstrdup (q);
      p++;
      r++;
      q = r;
    }
    if (*q) {
      *p = xstrdup (q);
      p++;
    }
    *p = NULL;
    free (v);
}
#endif /* WIN32 || __CYGWIN__ */

/* On win32 SELFAUTO{LOC,DIR,PARENT} must not be just `/', otherwise,
   e.g., $SELFAUTODIR/texmf/tex would be mistaken as UNC name.  */
static inline string
fix_selfdir (string dir)
{
#if defined(WIN32)
  if (IS_DIR_SEP_CH (*dir) && dir[1] == 0)
    dir++;
#endif
  return dir;
}

void
kpathsea_set_program_name (kpathsea kpse,  const_string argv0,
                           const_string progname)
{
  const_string ext;
  string sdir, sdir_parent, sdir_grandparent, sdir_greatgrandparent;
  string s = getenv ("KPATHSEA_DEBUG");
#ifdef WIN32
  string debug_output = getenv("KPATHSEA_DEBUG_OUTPUT");
  string append_debug_output = getenv("KPATHSEA_DEBUG_APPEND");
  int err, olderr, cp;
#endif

  /* Set debugging stuff first, in case we end up doing debuggable stuff
     during this initialization.  */
  if (s) {
    kpse->debug |= atoi (s);
  }

#if defined(WIN32)
  if (!kpse->File_system_codepage)
    kpse->File_system_codepage = AreFileApisANSI() ? GetACP() : GetOEMCP();
  cp = kpse->File_system_codepage;
  if (cp == 932 || cp == 936 || cp == 950) {
    kpse->Is_cp932_system = cp;
  }
  else
    kpse->Is_cp932_system = 0;

#if defined(__MINGW32__)
  /* Set various info about user. Among many things,
     ensure that HOME is set.  */
  init_user_info();
#else /* !__MINGW32__ */
  kpse->the_passwd.pw_name = kpse->the_passwd_name;
  kpse->the_passwd.pw_passwd = kpse->the_passwd_passwd;
  kpse->the_passwd.pw_uid = 0;
  kpse->the_passwd.pw_gid = 0;
  kpse->the_passwd.pw_quota = 0;
  kpse->the_passwd.pw_gecos = kpse->the_passwd_gecos;
  kpse->the_passwd.pw_dir = kpse->the_passwd_dir;
  kpse->the_passwd.pw_shell = kpse->the_passwd_shell;
  kpse->__system_allow_multiple_cmds = 0;

  /* Set various info about user. Among many things,
     ensure that HOME is set.  */
  kpathsea_init_user_info(kpse);
#endif /* !__MINGW32__ */

  /* redirect stderr to debug_output. Easier to send logfiles. */
  if (debug_output) {
    int flags =  _O_CREAT | _O_TRUNC | _O_RDWR;
    err = -1;
    if (_stricmp(debug_output, "con") == 0
       || _stricmp(debug_output, "con:") == 0) {
      err = _fileno(stdout);
    } else {
      if (append_debug_output) {
        flags =  _O_CREAT | _O_APPEND | _O_WRONLY;
      } else {
        flags =  _O_CREAT | _O_TRUNC | _O_WRONLY;
        kpathsea_xputenv(kpse, "KPATHSEA_DEBUG_APPEND", "yes");
      }
    }

    if ((err < 0)
        && (err = _open(debug_output, flags, _S_IREAD | _S_IWRITE)) == -1)
    {
      WARNING1 ("kpathsea: Can't open %s for stderr redirection!\n",
                debug_output);
      perror (debug_output);
    } else if ((olderr = _dup(fileno(stderr))) == -1) {
      WARNING ("kpathsea: Can't dup() stderr!\n");
      close (err);
    } else if (_dup2(err, fileno(stderr)) == -1) {
      WARNING1 ("kpathsea: Can't redirect stderr to %s!\n", debug_output);
      close (olderr);
      close (err);
    } else {
      close (err);
    }
  }

  /* Win95 always gives the short filename for argv0, not the long one.
     There is only this way to catch it. It makes all the kpse_selfdir
     stuff useless for win32. */
  {
    char short_path[PATH_MAX], path[PATH_MAX], *fp;
#if !defined(__MINGW32__)
    HANDLE hnd;
    WIN32_FIND_DATA ffd;
#endif

    /* SearchPath() always gives back an absolute directory */
    if (SearchPath(NULL, argv0, ".exe", PATH_MAX, short_path, &fp) == 0)
        LIB_FATAL1("Can't determine where the executable %s is.\n", argv0);
#if defined(__MINGW32__)
    if (!win32_get_long_filename(short_path, path, sizeof(path))) {
        LIB_FATAL1("This path points to an invalid file : %s\n", short_path);
    }
    /* slashify the dirname */
    for (fp = path; fp && *fp; fp++)
        if (IS_DIR_SEP(*fp)) *fp = DIR_SEP;
#else /* !__MINGW32__ */
    if (kpathsea_getlongpath(kpse, path, short_path, PATH_MAX) == 0)
        FATAL1("Can't get long name for %s.\n", short_path);
    if ((hnd = FindFirstFile(short_path, &ffd)) == INVALID_HANDLE_VALUE)
        FATAL1("The following path points to an invalid file : %s\n", path);
    FindClose(hnd);
    /* dirname aleady slashified in WIN32 */
#endif /* !__MINGW32__ */
    /* sdir will be the directory of the executable, ie: c:/TeX/bin */
    sdir = xdirname(path);
    kpse->invocation_name = xstrdup(xbasename(path));
  }

#elif defined(__DJGPP__)

  /* DJGPP programs support long filenames on Windows 95, but ARGV0 there
     is always made with the short 8+3 aliases of all the pathname elements.
     If long names are supported, we need to convert that to a long name.

     All we really need is to call `_truename', but most of the code
     below is required to deal with the special case of networked drives.  */
  if (pathconf (argv0, _PC_NAME_MAX) > 12) {
    char long_progname[PATH_MAX];

    if (_truename (argv0, long_progname)) {
      char *fp;

      if (long_progname[1] != ':') {
        /* A complication: `_truename' returns network-specific string at
           the beginning of `long_progname' when the program resides on a
           networked drive, and DOS calls cannot grok such pathnames.  We
           need to convert the filesystem name back to a drive letter.  */
        char rootname[PATH_MAX], rootdir[4];

        if (argv0[0] && argv0[1] == ':')
          rootdir[0] = argv0[0]; /* explicit drive in `argv0' */
        else
          rootdir[0] = getdisk () + 'A';
        rootdir[1] = ':';
        rootdir[2] = '\\';
        rootdir[3] = '\0';
        if (_truename (rootdir, rootname)) {
          /* Find out where `rootname' ends in `long_progname' and replace
             it with the drive letter.  */
          int root_len = strlen (rootname);

          if (IS_DIR_SEP (rootname[root_len - 1]))
            root_len--; /* keep the trailing slash */
          long_progname[0] = rootdir[0];
          long_progname[1] = ':';
          memmove (long_progname + 2, long_progname + root_len,
                   strlen (long_progname + root_len) + 1);
        }
      }

      /* Convert everything to canonical form.  */
      if (long_progname[0] >= 'A' && long_progname[0] <= 'Z')
        long_progname[0] += 'a' - 'A'; /* make drive lower case, for beauty */
      for (fp = long_progname; *fp; fp++)
        if (IS_DIR_SEP (*fp))
          *fp = DIR_SEP;

      kpse->invocation_name = xstrdup (long_progname);
    }
    else
      /* If `_truename' failed, God help them, because we won't...  */
      kpse->invocation_name = xstrdup (argv0);
  }
  else
    kpse->invocation_name = xstrdup (argv0);

#else /* !WIN32 && !__DJGPP__ */
  kpse->invocation_name = xstrdup (argv0);
#endif

  /* We need to find SELFAUTOLOC *before* removing the ".exe" suffix from
     the program_name, otherwise the PATH search inside kpse_selfdir will fail,
     since `prog' doesn't exists as a file, there's `prog.exe' instead.  */
#ifndef WIN32
  sdir = kpathsea_selfdir (kpse, kpse->invocation_name);
#endif
  /* SELFAUTODIR is actually the parent of the invocation directory,
     and SELFAUTOPARENT the grandparent.  This is how teTeX did it.  */
  kpathsea_xputenv (kpse, "SELFAUTOLOC", fix_selfdir (sdir));
  sdir_parent = xdirname (sdir);
  kpathsea_xputenv (kpse, "SELFAUTODIR", fix_selfdir (sdir_parent));
  sdir_grandparent = xdirname (sdir_parent);
  kpathsea_xputenv (kpse, "SELFAUTOPARENT", fix_selfdir (sdir_grandparent));
  sdir_greatgrandparent = xdirname (sdir_grandparent);
  kpathsea_xputenv (kpse, "SELFAUTOGRANDPARENT", fix_selfdir (sdir_greatgrandparent));

#if defined(WIN32) || defined(__CYGWIN__)
  mk_suffixlist(kpse);
#endif /* WIN32 || __CYGWIN__ */

  free (sdir);
  free (sdir_parent);
  free (sdir_grandparent);
  free (sdir_greatgrandparent);

  kpse->invocation_short_name
    = xstrdup (xbasename (kpse->invocation_name));

  if (progname) {
    kpse->program_name = xstrdup (progname);
  } else {
    /* If configured --enable-shared and running from the build directory
       with the wrapper scripts (e.g., for make check), the binaries will
       be named foo.exe instead of foo.  Or possibly if we're running on a
       DOSISH system.  */
    ext = find_suffix (kpse->invocation_short_name);
    if (ext && FILESTRCASEEQ (ext, "exe")) {
      kpse->program_name = remove_suffix (kpse->invocation_short_name);
    } else {
      kpse->program_name = xstrdup (kpse->invocation_short_name);
    }
  }

  /* Runtime check that snprintf always writes a trailing NUL byte.  */
  {
    char buf[4] = "old";
    assert (snprintf (buf, 2, "a") == 1 && buf[1] == '\0');
    assert ((unsigned)snprintf (buf, 2, "ab") >= 2 && buf[1] == '\0');
    assert ((unsigned)snprintf (buf, 2, "abc") >= 2 && buf[1] == '\0');
  }
  /* Some of the utility routines (like atou() and xfopen()) will use
     FATAL and variations thereof (see lib.h) if there is a problem.

     The next trick makes it possible for that message to report some useful
     name instead of (NULL), if the backward compatible is compiled in. */

#if defined (KPSE_COMPAT_API)
  if (kpse!=kpse_def) {
    kpse_def->invocation_name = xstrdup(kpse->invocation_name);
    kpse_def->invocation_short_name = xstrdup(kpse->invocation_short_name);
  }
#endif

  kpathsea_xputenv (kpse, "progname", kpse->program_name);
}


#if defined (KPSE_COMPAT_API)
void
kpse_set_program_name (const_string argv0, const_string progname)
{
  kpathsea_set_program_name (kpse_def, argv0, progname);
}
#endif

/* Returns ARGV0 with any leading path and on some systems the suffix
   for executables stripped off.  This returns a new string.
   For example, `kpse_program_basename ("/foo/bar.EXE")' returns "bar"
   on WIndows or Cygwin and "bar.EXE" otherwise.  */

string
kpse_program_basename (const_string argv0)
{
  string base = xstrdup (xbasename (argv0));
#ifdef EXEEXT
  string dot = strrchr (base, '.');
  if (dot && FILESTRCASEEQ (dot, EXEEXT))
    *dot = 0;
#endif
  return base;
}


#ifdef TEST
static const char *tab[] = {
/* 'normal' names */
    "/w/kpathsea",
    "/w//kpathsea",
    "/w/./kpathsea",
    ".",
    "./",
    "./.",
    "../kpathsea",
    "/kpathsea/../foo",
    "/../w/kpathsea",
    "/../w/kpathsea/.",
    "/te/share/texmf/../../../../bin/gnu",
    NULL
};

int
main (int argc, char **argv)
{
    const char **p;
    kpathsea kpse = xcalloc(1, sizeof(kpathsea_instance));

    kpathsea_set_program_name(kpse, argv[0], NULL);

#if defined(WIN32)
    printf("\n%s: Nothing to do for WIN32\n",
           kpse->invocation_short_name);
#else
    printf("\n%s: name -> remove_dots(name)\n\n",
           kpse->invocation_short_name);

    for (p = tab; *p; p++) {
        char *q = xstrdup(*p);
        char *s = remove_dots(kpse, q);

        printf("%s -> %s\n", q, s);
        free (q);
        free (s);
    }
#endif

    return 0;
}
#endif /* TEST */
