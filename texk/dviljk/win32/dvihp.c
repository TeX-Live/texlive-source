/* dvihp.c: emulate the dvihp shell script

   Converted to C by Eli Zaretskii <eliz@is.elta.co.il>
   Adapted to Win32 by Fabrice POPINEAU
   
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
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include <kpathsea/config.h>
#include <fcntl.h>
#ifdef _WIN32
#include <io.h>
#endif

#include <kpathsea/kpathsea.h>
#include "mktex.h"
#include "stackenv.h"

static const char rcs_id[] =
  "$Id: dvihp.c,v 1.7 2002/10/06 14:02:09 olaf Exp $";
static char rcs_revision[] = "$Revision: 1.7 $";

static void usage (void)
{
  printf ("Usage: %s [OPTIONS] [DVIFILE[.dvi]].\n\
  Translate the given DVIFILE to Hewlett-Packard PCL by calling dvicopy\n\
  and then $DVILJ (dvilj4 by default).\n\
  In the absence of other options, pipe the PCL to $SPOOL (lpr by default).\n\
\n\
  Options are recognized from dvips where possible:\n\
-A    print odd pages\n\
-B    print even pages\n\
-d #  set debug bits to # (see documentation)\n\
-D #  set resolution to #\n\
-f    run as filter\n\
-l #  don't print pages after #\n\
-m    manual feed\n\
-n #  print # pages\n\
-O #,#  set/change paper offset to #,# mm\n\
-o s  output to s instead of spooling\n\
-p #  don't print pages before #\n\
-Ps   pass directly to lpr\n\
-v    verbose operation\n\
-x #  set magnification to #\n\
\n\
Other options are passed to the dvilj program.\n\
\n\
Email bug reports to tex-k@mail.tug.org.\n", kpse_invocation_name);
  fflush (stdout);
}

/* Emulate `rm -f prefix*'  */


static void rmfiles (char *prefix)
{
#ifdef _WIN32
  /* Win32 doesn't have POSIX dirent functions.  */
  WIN32_FIND_DATA ffd;
  HANDLE hnd;
  int go_on;
  string temp = concat (prefix, "*");
  string directory = xdirname (prefix);

  pushd (directory);
  hnd = FindFirstFile(temp, &ffd);
  go_on = (hnd != INVALID_HANDLE_VALUE); 
  while (go_on) {
    /* FIXME: can this remove read-only files also, like rm -f does?  */
    DeleteFile(ffd.cFileName);
    go_on = (FindNextFile(hnd, &ffd) == TRUE);
  }
  FindClose(hnd);
  free(temp);
  popd ();
#else  /* not _WIN32 */
  DIR *dp;
  struct dirent *de;
  int temp_len = strlen (prefix);
  string directory = "./";
  const_string base = xbasename (prefix);

  /* Copy the directory part of PREFIX with the trailing slash, if any.  */
  if (base != prefix)
    {
      directory = (string) xmalloc (base - prefix + 1);
      directory[0] = '\0';
      strncat (directory, prefix, base - prefix);
    }

  /* Find matching files and delete them.  */
  if ((dp = opendir (directory)) != 0) {
    while ((de = readdir (dp)) != 0)
      {
	string found = concat (directory, de->d_name);

	if (FILESTRNCASEEQ (found, prefix, temp_len))
	  /* On POSIX-compliant systems, including DJGPP, this will also
	     remove read-only files and empty directories, like rm -f does.  */
	  if (remove (found))
	    perror (found);
	free (found);
      }
  }
#endif /* not _WIN32 */
}

static void __cdecl output_and_cleanup(int code)
{
  /* FIXME : what cleanup should be done ? */
}

char *progname;

int
main (int argc, char *argv[])
{
  string opt	= (string) xmalloc (1);
  const_string output = "", infile = "";
  const_string output_opt = " -e";
  const_string dvilj;
  const_string spool;
  const_string tmpdir;
  const_string spool_opt = "";
  char template[PATH_MAX];
  const_string vfless_dvi;
  int verbose = 0;
  string cmdline;
  const_string maybe_spool_cmd = "";
  const_string space = " ";

  output_and_cleanup_function = output_and_cleanup;

  progname = argv[0];
  kpse_set_program_name (argv[0], "dvihp");
#ifdef _WIN32
  setmode(fileno(stdin), _O_BINARY);
  setmode(fileno(stdout), _O_BINARY);
  SetConsoleCtrlHandler((PHANDLER_ROUTINE)mt_exit, TRUE);
#else
# ifdef SIGINT
  signal (SIGINT, sigint_handler);
# endif
# ifdef SIGQUIT
  signal (SIGQUIT, sigint_handler);
# endif
# ifdef SIGHUP
  signal (SIGHUP, sigint_handler);
# endif
# ifdef SIGTERM
  signal (SIGTERM, sigint_handler);
# endif
#endif

  dvilj  = kpse_var_value ("DVILJ");
  spool  = kpse_var_value ("SPOOL");
  tmpdir = kpse_var_value ("TMPDIR");
  opt[0] = '\0';

  if(!tmpdir)
    tmpdir = kpse_var_value ("TMP");
  if(!tmpdir)
    tmpdir = kpse_var_value ("TEMP");

  /* Set default values for variables:

     ${DVILJ=dvilj4} the dvilj variant to run
     ${SPOOL=lpr}    used to print an LJ file
     ${TMPDIR=/tmp}  for the dvicopy output    */
  if (!dvilj || !*dvilj)
    dvilj = "dvilj4";
  if (!spool || !*spool)
    {
#ifdef __DJGPP__
      /* Feature: if they don't have LPR installed, make the
	 default be to print to the local printer device.  */
      char lpr_path[PATH_MAX];
      extern char *__dosexec_find_on_path (const char *, char **, char *);
      extern char **environ;

      if (!__dosexec_find_on_path ("lpr", (char **)0, lpr_path)
	  && !__dosexec_find_on_path ("lpr", environ, lpr_path))
	spool = "> PRN";
      else
#endif
	spool = "| lpr";
    }

  if (!tmpdir || !*tmpdir)
    {
#ifdef _WIN32
      tmpdir = "c:/tmp";
#else
      tmpdir = "/tmp";
#endif
      if (access (tmpdir, R_OK|W_OK) != 0)
	{
	  mkdir (tmpdir, S_IRWXU);
	}
    }

  if (argc <= 1)
    {
      fprintf (stderr, "%s: Missing argument(s).\nTry `%s --help' for more information.\n",
	       kpse_invocation_name, kpse_invocation_name);
      return 0;
    }

  while (--argc)
    {
      char *arg = *++argv;

      if (STREQ (arg, "-help") || STREQ (arg, "--help"))
	{
	  usage ();
	  return 0;
	}
      else if (STREQ (arg, "-version") || STREQ (arg, "--version"))
	{
	  printf ("%s: (Dviljk 2.6) %s\nThere is NO warranty.  This program is public domain.\n",
		  kpse_invocation_name, rcs_revision);
	  return 0;
	}
      else if (arg[0] == '-')
	{
	  string newopt = NULL;

	  switch (arg[1])
	    {
	      case 'A':	/* -A => -D1 (odd pages) */
		newopt = concat (opt, " -D1");
		break;
	      case 'B': /* -B -> -D2 (even pages) */
		newopt = concat (opt, " -D2");
		break;
	      case 'd':	/* -d => --D (debug) */
		if (arg[2] == '\0')
		  {
		    argc--;
		    newopt = concat3 (opt, " --D", *++argv);
		  }
		else
		  newopt = concat3 (opt, " --D", arg + 2);
		break;
	      case 'D':	/* -D => -R (resolution) */
		argc--;
		newopt = concat3 (opt, " -R", *++argv);
		break;
	      case 'f':	/* -f (run as filter) */
		infile = "";
		output = "-";
		break;
	      case 'l':	/* -l => -t (ending page) */
		if (arg[2] == '\0')
		  {
		    argc--;
		    newopt = concat3 (opt, " -t", *++argv);
		  }
		else
		  newopt = concat3 (opt, " -t", arg + 2);
		break;
	      case 'm':	/* -m => -A (manual feed) */
		newopt = concat (opt, " -A");
		break;
	      case 'n':	/* -n => -p (page count) */
		if (arg[2] == '\0')
		  {
		    argc--;
		    newopt = concat3 (opt, " -t", *++argv);
		  }
		else
		  newopt = concat3 (opt, " -t", arg + 2);
		break;
	      case 'o':	/* -o (output file) */
		{
		  string inbase = xstrdup (xbasename (infile));
		  if (argc == 1 && *inbase)
		    {
		      /* No remaining args, output to foo.lj. */
		      int inbaselen = strlen (inbase);

		      if (FILESTRCASEEQ (inbase + inbaselen - 4, ".dvi"))
			output = strcpy (inbase + inbaselen - 4, ".lj");
		      else
			{
#if defined (MSDOS) && defined _PC_NAME_MAX
			  if (pathconf (infile, _PC_NAME_MAX) <= 12)
			    {
			      /* Plain MS-DOS (not Windows 95), can't have
				 more than a single dot in a filename.  */
			      char *dot = strchr (inbase, '.');

			      if (dot)
				*dot = '\0';
			    }
#endif
			  output = concat (inbase, ".lj");
			  free (inbase);
			}
		    }
		  else if (arg[2] == '\0')
		    {
		      argc--;
		      output = *++argv;
		    }
		  else
		    output = arg + 2;
		  break;
		}
	      case 'O':	/* -O => -x, -y (page offsets) */
		{
		  int x_len;
		  char *params = arg + 2;
		  char *x;

		  if (arg[2] == '\0')
		    {
		      argc--;
		      params = *++argv;
		    }

		  x_len = strcspn (params, ",");
		  x = (char *)xmalloc (x_len + 1);
		  x[0] = '\0';
		  strncat (x, params, x_len);
		  if (params[x_len] == ',')
		    x_len++; /* get past the comma, if any */
		  newopt = concatn (opt, " -x", x, " y", params + x_len, NULL);
		  break;
		}
	      case 'p':	/* -p => -f (starting page) */
		if (arg[2] == '\0')
		  {
		    argc--;
		    newopt = concat3 (opt, " -f", *++argv);
		  }
		else
		  newopt = concat3 (opt, " -f", arg + 2);
		break;
	      case 'P':	/* -Pprinter */
		output = "";
		if (spool[0] == '>') /* direct printing: replace device */
		  spool = concat ("> ", arg[2] ? arg + 2 : *++argv);
		else
		  spool_opt = concat ("-P", arg[2] ? arg + 2 : *++argv);
		if (arg[2] == '\0')
		  argc--;
		break;
	      case 'v':	/* verbose */
		verbose = 1;
		newopt = concat (opt, " -v");
		break;
	      case 'x':	/* -x => -m (magnification) */
		if (arg[2] == '\0')
		  {
		    argc--;
		    newopt = concat3 (opt, " -m", *++argv);
		  }
		else
		  newopt = concat3 (opt, " -m", arg + 2);
		break;
	      case '-':	/* -- => end of options */
		argc--;
		infile = *++argv;
		break;
	      default:	/* pass other options through */
		newopt = concat3 (opt, space, arg);
		break;
	    }

	  if (newopt)
	    {
	      free (opt);
	      opt = newopt;
	    }
	}
      else
	infile = arg;
    }

  /* Generate temporary filenames.  Assumes PID is 5 digits.  */
  sprintf (template, "%s/dvi%5.5d.", tmpdir, getpid ());
  vfless_dvi = concat (template, "-vf");

  /* Expand VF references.
     If INFILE is null, this will read standard input.
     dvilj can't read from a pipe, so always write to a file.  */
  if (verbose)
    fprintf (stderr, "Running dvicopy %s >%s\n", infile, vfless_dvi);

  /* FIXME: special characters in filenames aren't protected from the shell. */
  cmdline = concatn ("dvicopy ", infile, " >", vfless_dvi, NULL);
  if (system (cmdline))
    {
      fprintf (stderr, "%s: dvicopy %s failed.\n", progname, infile);
      return 1;
    }
  free (cmdline);

  if (verbose)
    {
      struct stat st_buf;

      if (stat (vfless_dvi, &st_buf))
	perror (vfless_dvi);
      else	/* assume they only want to see the size and timestamp */
	fprintf (stderr, " %ld  %s  %s\n",
		 (long)st_buf.st_size, ctime (&st_buf.st_mtime), vfless_dvi);
    }
  if (!*output)
    {
      output = "-";	/* output to stdout */
      maybe_spool_cmd = concat (spool, spool_opt);
    }

  /* Translate DVI to LJ.  */
  cmdline = concatn (dvilj, space, opt, output_opt, output,
		     space, vfless_dvi, space, maybe_spool_cmd, NULL);
  if (verbose)
    fprintf (stderr, "Running %s\n", cmdline);
  if (system (cmdline))
    {
      fprintf (stderr, "%s: %s failed.\n", progname, dvilj);
      return 2;
    }

  /* rm -f $TMPDIR/dvi$$.*  */
  rmfiles (template);

  return 0;
}
