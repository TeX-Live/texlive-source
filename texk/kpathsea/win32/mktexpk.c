/* mktexpk.c

   Copyright 2000, 2015 Akira Kakuto.

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

 %%----------------------------------------
 % Special variables for mktexpk ( W32TeX )
 % ----------------------------------------
 % MAKETEXPK_STYLE = dosnames
 % -----------------------------------------
 % MAKETEXPK_TOP_DIR = $VARTEXFONTS
 % -----------------------------------------
 % MAKETEXPK_MODE = canonex
 % -----------------------------------------
 % MAKETEXPK_MODE_300 = cx
 % MAKETEXPK_MODE_400 = nexthi
 % MAKETEXPK_MODE_600 = canonex
 % etc.
 %%-----------------------------------------

 Usage: mktexpk [OPTIONS] name,
   Create a PK font.

 --dpi DPI           use resolution DPI.
 --bdpi BDPI         use base resolution BDPI.
 --mag MAG           use magnificiation MAG.
 --mfmode MODE       use MODE as the METAFONT mode.
 --destdir DESTDIR   write fonts in DESTDIR (absolute path).

 The following old form is also supported:
 Usage: mktexmk name dpi bdpi mag [mode]
*/

#include <kpathsea/kpathsea.h>
#ifndef __MINGW32__
#include <web2c/w2c/c-auto.h>
#endif
#include "mktex.h"

#define LLBUF 1024
#define LBUF  512
#define SBUF  512
#define TBUF  512

/*
Global variables
*/

char *progname;
int UseAspectRatio = 0;
int FileName = 0;
double AspectRatio = 1.0;

#define BBUFF_FUL     253
#define FFILE_END     254
#define LLINE_END     255

static int
ffgets(char *buf, int size, FILE *fi)
{
  char *p;
  int  c;
  int  n;

  n = 0;
  p = buf;
  *p = '\0';

  while(1) {
    c = getc(fi);
    if(c == 26)
      continue;
    n++;
    if(c == EOF) {
      n++;
      *p++ = '\n';
      *p = '\0';
      return FFILE_END;
    }
    else if(c == 0x0d || c == 0x0a) {
      n++;
      *p++ = '\n';
      *p = '\0';
      if(c == 0x0d) {
        c = getc(fi);
        if(c != 0x0a)
          ungetc(c, fi);
      }
      return LLINE_END;
    }
    else if(n == (size - 3)) {
      *p++ = c; *p = '\0';
      return BBUFF_FUL;
    }
    else *p++ = c;
  }
}

static int
isskip(char c)
{
  if((c == ' ') || (c == '\t'))
    return 1;
  else if((c == '<') || (c == '[')) {
    FileName = 1;
    return 1;
  }
  else
    return 0;
}

static void
skipchar(char **p)
{
  while(isskip(**p))
    (*p)++;
}

static void
version (void)
{
  fprintf (stderr, "%s, (C version 1.5 --ak 2006-2012)\n", progname);
  fprintf (stderr, KPSEVERSION WEB2CVERSION "\n");
}

static void
usage (void)
{
  fprintf (stderr, "Usage: %s [OPTIONS] NAME,\n\
  Create a PK font.\n\n\
--dpi DPI           use resolution DPI.\n\
--bdpi BDPI         use base resolution BDPI.\n\
--mag MAG           use magnificiation MAG.\n\
--mfmode MODE       use MODE as the METAFONT mode.\n\
--destdir DESTDIR   write fonts in DESTDIR.\n", progname);
}

static void
help (void)
{
  fprintf (stderr, "Usage: %s [OPTIONS] NAME,\n\
  Create a PK font.\n\n\
--dpi DPI           use resolution DPI.\n\
--bdpi BDPI         use base resolution BDPI.\n\
--mag MAG           use magnificiation MAG.\n\
--mfmode MODE       use MODE as the METAFONT mode.\n\
--destdir DESTDIR   write fonts in DESTDIR.\n\n\
Try to create a PK file for NAME at resolution DPI, with an assumed\n\
device base resolution of BDPI, and a Metafont `mag' of MAG. Use MODE\n\
for the METAFONT mode.  Use DESTDIR for the root of where to install\n\
into. DESTDIR must be an absolute directory name. If DESTDIR is not given,\n\
suitable directory is determined according to the TDS.\n\n\
Old form:\n\
Usage: %s name dpi bdpi mag [mode]\n\
is also supported.", progname, progname);
}


static void
tpkerr (const char *s)
{
  fprintf (stderr, "%s\n", s);
}


static void
relmem (char **v)
{
  int j;
  for (j = 0; j < 4; j++)
    free (v[j]);
  return;
}

static void
get_designsize(char *tfname, char *dsiz)
{
  FILE *f;
  unsigned char a, b, c, d;
  unsigned int  n;
  double   x;
  char     *p;

  strcpy(dsiz, "10.0"); /* default */

  p = kpse_find_file (tfname, kpse_tfm_format, 0);
  if(p) {
    f = fopen(p, "rb");
    free(p);
    if(f) {
      fseek(f, 28, SEEK_SET);
      a = getc(f);
      b = getc(f);
      c = getc(f);
      d = getc(f);
      fclose(f);
      n = (a << 24) + (b << 16) + (c << 8) + d;
      x = n / 1048576.0;
      sprintf(dsiz, "%.1lf", x);
    }
  }
}

int
main (int ac, char **av)
{
  static char execfile[SBUF];
  char rbuff[LBUF];
  char buff[LBUF];
  char cmd[LBUF];
  char mfname[TBUF];
  char tfname[TBUF];
  char pkname[TBUF];

  char name[TBUF];
  char dpi[TBUF];
  char ydpi[TBUF];
  char bdpi[TBUF];
  char mag[TBUF];
  char mode[TBUF];
  char destdir[SBUF];
  char designsize[64];

  char *arg[4];

  char currdir[SBUF];
  char kpsedot[SBUF];
  char *tmp;
  int cdrive, tdrive;

  FILE *fr, *fw, *fnul, *tfmfileptr;

  int i, savo, savi, ret;
  int style;
  int issetdest;
  int app;
  int oldform;
  int ps2pkok;
  char *env;
  char *p, *fpp;

  double Xdpi, Ydpi;

  char texname[TBUF], pfbname[TBUF], slant[TBUF], extend[TBUF], encname[TBUF];

  char texbindir[256];
  char fullbin[512];

/*
 * style =  0 : MAKETEXPK_STYLE undefined or other than dosnames
 * style =  1 : MAKETEXPK_STYLE = dosnames
 */

/*
 * issetdest = 0 : no destdir
 * issetdest = 1 : destdir
 * issetdest = 2 : current working dir
 */

/*
 * app = 0 : mf
 * app = 1 : ps2pk
 * app = 2 : gsftopk
 * app = 3 : ttf2pk
 * app = 4 : hbf2gf
 */

/*
 * oldform = 0 : newform of the command line
 * oldform = 1 : oldform of the command line
 */

/*
 * TEMP | TMP | TMPDIR (necessary)
 *
 */

  tmp = getenv ("TEMP");
  if (!tmp)
    tmp = getenv ("TMP");
  if (!tmp)
    tmp = getenv ("TMPDIR");
  if (!tmp) {
    tpkerr ("Please define TEMP | TMP | TMPDIR.");
    return (100);
  }
  tmp = xstrdup(tmp);

/*
 * normalize directory separators
 */
  normalize (tmp);

  for (i = 0; i < 4; i++)
    arg[i] = (char *) malloc (SBUF);

  kpse_set_program_name (av[0], NULL);
  progname = kpse_program_name;

/*
 * get tex binary dir
 *
 */
  p = kpse_var_value("SELFAUTOLOC");
  if(p == 0) {
     fprintf(stderr, "I cannot get SELFAUTOLOC\n");
     exit(100);
  }
  strcpy(texbindir, p);
  free(p);
  for(p=texbindir; *p; p++) {
     if(*p == '/') *p = '\\';
  }
  *p = '\\';
  *(p+1) = '\0';

  if (ac < 2) {
    usage ();
    relmem (arg);
    free(tmp);
    return (100);
  }

  issetdest = 0;
  ps2pkok = 0;

/*
 * oldform or newform ?
 *
 */

  if (av[1][0] == '-')
    oldform = 0;
  else
    oldform = 1;


/*
 * Old form of the command line
 */

  if (oldform == 1) {
    if (ac < 5) {
      usage ();
      relmem (arg);
      free(tmp);
      return (100);
    }
    if((strlen(av[1]) > TBUF -1 ) ||
       (strlen(av[2]) > TBUF -1 ) ||
       (strlen(av[3]) > TBUF -1 ) ||
       (strlen(av[4]) > TBUF -1 )) {
      fprintf(stderr, "\nToo long a string.\n");
      free(tmp);
      return 100;
    }

    strcpy (name, av[1]);
    strcpy (dpi, av[2]);
    strcpy (bdpi, av[3]);
    strcpy (mag, av[4]);
    if (ac > 5) {
      if(strlen(av[5]) > TBUF -1) {
        fprintf(stderr, "\nToo long a string.\n");
        free(tmp);
        return 100;
      } 
      strcpy (mode, av[5]);
    }
    else
      mode[0] = '\0';
  } else {
/*
 * New form of the command line
 */
    name[0] = dpi[0] = bdpi[0] = mag[0] = mode[0] = destdir[0] = '\0';
    i = 1;
    while (i < ac) {
      if(strlen(av[i]) > TBUF - 1) {
        fprintf(stderr, "\nToo long a string.\n");
        free(tmp);
        return 100;
      }
      if (av[i][0] != '-') {
        strcpy (name, av[i]);
        break;
      }
      if (!strcmp (av[i], "--dpi") || !strcmp (av[i], "-dpi")) {
        i++;
        if (i >= ac) {
          tpkerr ("Invalid arguments.");
          relmem (arg);
          free(tmp);
          return (100);
        }
        strcpy (dpi, av[i]);
        i++;
      } else if (!strcmp (av[i], "--bdpi") || !strcmp (av[i], "-bdpi")) {
        i++;
        if (i >= ac) {
          tpkerr ("Invalid arguments.");
          relmem (arg);
          free(tmp);
          return (100);
        }
        strcpy (bdpi, av[i]);
        i++;
      } else if (!strcmp (av[i], "--mag") || !strcmp (av[i], "-mag")) {
        i++;
        if (i >= ac) {
          tpkerr ("Invalid arguments.");
          relmem (arg);
          free(tmp);
          return (100);
        }
        strcpy (mag, av[i]);
        i++;
      } else if (!strcmp (av[i], "--mfmode") || !strcmp (av[i], "-mfmode")) {
        i++;
        if (i >= ac) {
          tpkerr ("Invalid arguments.");
          relmem (arg);
          free(tmp);
          return (100);
        }
        strcpy (mode, av[i]);
        i++;
      } else if (!strcmp (av[i], "--destdir") || !strcmp (av[i], "-destdir")) {
        i++;
        if (i >= ac) {
          tpkerr ("Invalid arguments.");
          relmem (arg);
          free(tmp);
          return (100);
        }
        strcpy (destdir, av[i]);
        issetdest = 1;
        i++;
      } else if (!strcmp (av[i], "--version") || !strcmp (av[i], "-version")) {
        version ();
        relmem (arg);
        free(tmp);
        return (0);
      } else if (!strcmp (av[i], "--help") || !strcmp (av[i], "-help")) {
        help ();
        relmem (arg);
        free(tmp);
        return (0);
      } else {
        tpkerr ("Argument error.");
        relmem (arg);
        free(tmp);
        return (100);
      }
    }
  }                             /* End of command line analysis */

  env = kpse_var_value ("MAKETEXPK_STYLE");

  if ((env == NULL) || !(*env) || (env && strcmp (env, "dosnames"))) {
    style = 0;
  } else
    style = 1;

/*
 * Default program is mf
 */

  app = 0;

/*
 * check if mfmode and bdpi are consistent or not
 */

  if (bdpi[0] && mode[0] && mode[0] != '/') {
    FILE *frd;
    char buff[128];
    int len;

    strcpy (fullbin, texbindir);
    strcat (fullbin, "mf-nowin.exe \"\\mode:=");
    strcat (fullbin, mode);
    strcat (fullbin, ";mode_setup;message");
    strcat (fullbin, "(decimal round pixels_per_inch);");
    strcat (fullbin, "end. <nul\"");

    frd = popen (fullbin, "r");
    if (!frd) {
      tpkerr ("I cannot find METAFONT.\n");
      relmem (arg);
      free(tmp);
      return (100);
    }
    (void) fgets (buff, 126, frd);
    (void) fgets (buff, 126, frd);
    pclose (frd);
    system("del /Q mfput.*");

    len = (int)strlen (buff);
    if (buff[len - 1] == '\n') {
      buff[len - 1] = '\0';
      if (buff[len - 2] == '\r')
        buff[len - 2] = '\0';
    }
    if (strcmp (bdpi, buff)) {
      fprintf(stderr, "mode_dpi %s and bdpi %s are inconsistent.\n", buff, bdpi);
      fprintf(stderr, "therefore I reset mfmode.\n");
      mode[0] = '\0';
    }
  }

/*
 * determine mfmode if not given
 */

  if (mode[0] == 0 || mode[0] == '/') {
    if (bdpi[0] == 0) {
      tpkerr ("Cannot determine the mode.");
      tpkerr ("I will try other possibilities.");
      app = 1;
    } else {
      strcpy (rbuff, "MAKETEXPK_MODE_");
      strcat (rbuff, bdpi);
      if ((env = kpse_var_value ("MAKETEXPK_MODE")) && *env)
        strcpy (mode, env);
      else if ((env = kpse_var_value (rbuff)))
        strcpy (mode, env);
      else if (!strcmp (bdpi, "85"))
        strcpy (mode, "sun");
      else if (!strcmp (bdpi, "100"))
        strcpy (mode, "nextscrn");
      else if (!strcmp (bdpi, "118"))
        strcpy (mode, "pcprevw");
      else if (!strcmp (bdpi, "160"))
        strcpy (mode, "nectzo");
      else if (!strcmp (bdpi, "180"))
        strcpy (mode, "toshiba");
      else if (!strcmp (bdpi, "200"))
        strcpy (mode, "highfax");
      else if (!strcmp (bdpi, "240"))
        strcpy (mode, "canonlbp");
      else if (!strcmp (bdpi, "300"))
        strcpy (mode, "cx");
      else if (!strcmp (bdpi, "320"))
        strcpy (mode, "neclm");
      else if (!strcmp (bdpi, "360"))
        strcpy (mode, "epstylus");
      else if (!strcmp (bdpi, "400"))
        strcpy (mode, "nexthi");
      else if (!strcmp (bdpi, "600"))
        strcpy (mode, "ljfour");
      else if (!strcmp (bdpi, "720"))
        strcpy (mode, "epscszz");
      else if (!strcmp (bdpi, "800"))
        strcpy (mode, "lwpro");
      else if (!strcmp (bdpi, "1000"))
        strcpy (mode, "lmaster");
      else if (!strcmp (bdpi, "1200"))
        strcpy (mode, "ultre");
      else if (!strcmp (bdpi, "1270"))
        strcpy (mode, "linoone");
      else if (!strcmp (bdpi, "1800"))
        strcpy (mode, "vtftzz");
      else if (!strcmp (bdpi, "2400"))
        strcpy (mode, "supre");
      else if (!strcmp (bdpi, "2540"))
        strcpy (mode, "linotzzh");
      else if (!strcmp (bdpi, "3386"))
        strcpy (mode, "linolttz");
      else if (!strcmp (bdpi, "8000"))
        strcpy (mode, "dpdfezzz");
      else if (!strcmp (bdpi, "9600"))
        strcpy (mode, "ibx");
      else {
        tpkerr ("Cannot determine the mode.");
        tpkerr ("I will try other possibilities.");
        app = 1;
      }
    }
  }

  if (env) free (env);

  if (name[0] == 0) {
    tpkerr ("Font name is not given.");
    relmem (arg);
    free(tmp);
    return (100);
  }

  if ((p = strrchr (name, '.')))
    *p = '\0';

  strcpy (mfname, name);
  strcat (mfname, ".mf");

  if (app == 0) {
    if (!(p = kpse_var_value ("MFINPUTS"))) {
      tpkerr ("Cannot get value of MFINPUTS.");
      relmem (arg);
      free(tmp);
      return (100);
    }
    free (p);
    xputenv("MKTEXMF", "1");
    if (!(p = kpse_find_file (mfname, kpse_mf_format, 1))) {
      fprintf (stderr, "Cannot find %s .\n", mfname);
      tpkerr ("I try ps2pk --> gsftopk --> ttf2pk --> hbf2gf.");
      app = 1;
    }
  }

  if (app != 0) {
    strcpy (mode, "modeless");
    strcpy (tfname, name);
    strcat (tfname, ".tfm");
    if (!(p = kpse_var_value ("TFMFONTS"))) {
      tpkerr ("Cannot get value of TFMFONTS.");
      relmem (arg);
      free(tmp);
      return (100);
    }
    free (p);
/*
 I don't try to create nonexisting tfm here.
*/
    if (!(p = kpse_find_file (tfname, kpse_tfm_format, 0))) {
      fprintf (stderr, "Cannot find %s .\n", tfname);
      relmem (arg);
      free(tmp);
      return 100;
    }
    tfmfileptr = fopen (p, "rb");
    if (!tfmfileptr) {
      fprintf (stderr, "I cannot open %s.\n", p);
      relmem (arg);
      free(tmp);
      return 100;
    }
    i = 256 * getc (tfmfileptr);
    i += getc (tfmfileptr);
    fclose (tfmfileptr);
    if ((i == 9) || (i == 11)) {
      fprintf (stderr, "Current font seems to be a Japanese one.\n");
      fprintf (stderr, "I give up to create a PK font.\n");
      relmem (arg);
      free(tmp);
      return 100;
    }
  }

  if ((p[0] == '.') && (p[1] == '/') && (issetdest != 1))
    issetdest = 2;

  fpp = _getcwd (currdir, SBUF);
  if (!fpp) {
    fprintf (stderr, "Failed to get current working directory.\n");
    relmem (arg);
    free(tmp);
    return (100);
  }
  normalize (currdir);

  i = (int)strlen (currdir);
  if (currdir[i - 1] == '/')
    currdir[i - 1] = '\0';

  strcpy (kpsedot, "KPSE_DOT=.;");
  strcat (kpsedot, currdir);
  _putenv (kpsedot);

  if (issetdest == 2) {
    strcpy (destdir, currdir);
  }

  if (issetdest == 0) {
    strcpy (arg[0], "Dummy");
    strcpy (arg[1], "pk");
    strcpy (arg[2], p);
    strcpy (arg[3], mode);

    if (!(p = getdestdir (4, arg))) {
      tpkerr ("Cannot get destination directory name.");
      relmem (arg);
      free(tmp);
      return (100);
    }
    strcpy (rbuff, p);
  } else
    strcpy (rbuff, destdir);

/*
 * Change backslash into slash
 */
  normalize (rbuff);

  p = rbuff;
  i = (int)strlen (p);
  if (p[i - 1] == '/')
    p[i - 1] = '\0';

  if (issetdest) {
    if (!is_dir (p)) {
      fprintf (stderr, "Destination %s is not found.\n", p);
      relmem (arg);
      free(tmp);
      return (100);
    }
  } else if (!is_dir (p)) {
    if (make_dir (p)) {
      tpkerr ("Error in make_dir.");
      relmem (arg);
      free(tmp);
      return (100);
    }
  }

  strcpy (buff, p);
  p = buff;

  i = (int)strlen (p);

  if (p[i - 1] != '/')
    strcat (p, "/");

  if (dpi[0] == 0) {
    tpkerr ("Cannot determine DPI.");
    relmem (arg);
    free(tmp);
    return (100);
  }

  if (style == 1 && issetdest != 2) {   /* dosnames */
    strcat (p, "dpi");
    strcat (p, dpi);
    if (!is_dir (p)) {
      if (make_dir (p)) {
        tpkerr ("Error in make_dir.");
        relmem (arg);
        free(tmp);
        return (100);
      }
    }
    strcat (p, "/");
  }

  strcat (p, name);
  strcat (p, ".");

  if (style != 1 || issetdest == 2)
    strcat (p, dpi);            /* long suffix */
  strcat (p, "pk");

/* Now buff and p is the full path name of pk file */
/* check the existence of pk file */

  if (_access (p, 0) == 0) {
    fprintf (stderr, "%s exists.\n", p);
    relmem (arg);
    printf ("%s\n", p);
    free(tmp);
    return (0);
  }

/*
 * Go to the temporary directory
 */

  cdrive = _getdrive ();
  if (tmp[1] == ':') {
    tdrive = tolower (*tmp) - 'a' + 1;
    _chdrive (tdrive);
  }
  _chdir (tmp);

/*
 * save stdout and stdin
 */
  savo = _dup (fileno (stdout));
  savi = _dup (fileno (stdin));

/*
 * connect stdout to stderr
*/
  _dup2 (fileno (stderr), fileno (stdout));

/*
 * connect stdin to nul
 */
  if (!(fnul = fopen ("nul", "rb"))) {
    fprintf (stderr, "Cannot open nul device to read.\n");
    _chdrive (cdrive);
    _chdir (currdir);
    relmem (arg);
    free(tmp);
    return (100);
  }
  _dup2 (fileno (fnul), fileno (stdin));

/*
 * pkname is the filename of PK font
 */
  sprintf (pkname, "%s.%spk", name, dpi);

  if (app == 0) {
/*
 * METAFONT command line
 */
    if (mag[0] == 0) {
      tpkerr ("Cannot determine MAG.");
      _chdrive (cdrive);
      _chdir (currdir);
      relmem (arg);
      free(tmp);
      return (100);
    }
    sprintf (cmd,
       "--progname=mf --base=mf \\mode:=%s; \\mag:=%s; nonstopmode; input %s;",
        mode, mag, name);

    strcpy (execfile, "mf-nowin.exe");
    fprintf (stderr, "%s %s\n", execfile, cmd);
    strcpy(fullbin, texbindir);
    strcat(fullbin, execfile);
    (void) _spawnlp (_P_WAIT, fullbin, execfile, cmd, NULL);

    sprintf (cmd, "%s.%sgf", name, dpi);

/*
 * check the consistency
 */
    if (_access (cmd, 0) != 0) {
      tpkerr ("Failed to make gf font by METAFONT.");
      _chdrive (cdrive);
      _chdir (currdir);
      relmem (arg);
      free(tmp);
      return (100);
    }

/*
 * Change gf into pk
 */
    strcpy (execfile, "gftopk.exe");
    fprintf (stderr, "%s %s %s\n", execfile, cmd, pkname);
    strcpy(fullbin, texbindir);
    strcat(fullbin, execfile);
    (void) _spawnlp (_P_WAIT, fullbin, execfile, cmd, pkname, NULL);

    if (_access (pkname, 0) != 0) {
      tpkerr ("Failed to make pk from gf.");
      _chdrive (cdrive);
      _chdir (currdir);
      relmem (arg);
      free(tmp);
      return (100);
    }

/*
 * erase gf file
 */
    remove (cmd);

/*
 * erase log file
 */
    sprintf (cmd, "%s.log", name);
    remove (cmd);
/*
 * erase tfm file
 */
    sprintf (cmd, "%s.tfm", name);
    remove (cmd);

    goto finale;
  }

/*
 * app = 1 : ps2pk --> gsftopk --> ttf2pk --> hbf2gf
 */

  p = kpse_find_file ("pspksupp.map", kpse_fontmap_format, 0);
  if(p) {
    fr = fopen (p, "r");        /* Read pspksupp.map */
    free (p);

    if (!fr) {
      tpkerr ("Cannot open pspksupp.map to read.");
      ps2pkok = 0;
      goto do_ps2pk;
    }

    while (fgets (rbuff, SBUF, fr)) {
      if (rbuff[0] == '%' || rbuff[0] == '#' || rbuff[0] == '\n')
        continue;
      texname[0] = pfbname[0] = slant[0] = extend[0] = encname[0] = '\0';
      i = sscanf (rbuff, "%s %s %s %s %s", texname, pfbname, slant, extend,
                  encname);
      if (i == 2 && !strncmp (texname, "AspectRatio", 11)) {
        if (!sscanf (pfbname, "%lf", &AspectRatio)) {
          tpkerr ("File format of pspksupp.map is wrong.");
          fclose (fr);
          ps2pkok = 0;
          goto do_ps2pk;
        }
        UseAspectRatio = 1;
        continue;
      } else if (i > 0 && !stricmp (texname, name)) {
        p = kpse_var_value ("T1FONTS");
        if (!p) {
          tpkerr ("T1FONTS is not defined.");
          ps2pkok = 0;
          break;
        }
        free (p);
        p = kpse_find_file (pfbname, kpse_type1_format, 0);
        if (!p) {
          fprintf (stderr, "%s is not found.\n", pfbname);
          ps2pkok = 0;
          break;
        }
        free (p);
        ps2pkok = 1;
        if(bdpi[0] == 0)
          i--;
        break;
      }
    }
    fclose (fr);
    goto do_ps2pk;
  } else {
    char *q;
    char a[SBUF];
    char b[SBUF];
    char psname[SBUF];
    char pscommand[SBUF];
    double slantval, extendval;

    texname[0] = pfbname[0] = encname[0] = '\0';
    a[0] = b[0] = psname[0] = pscommand[0] = '\0';
    FileName = 0;
    strcpy(slant, "0");
    strcpy(extend, "1");

    ps2pkok = 0;

    p = kpse_find_file ("ps2pk.map", kpse_fontmap_format, 0);
    if(!p) {
      tpkerr("Necessary map file for ps2pk is not found.");
      goto do_ps2pk;
    }
    fr = fopen(p,"rb");
    free(p);
    if (!fr) {
      tpkerr ("Cannot open ps2pk.map to read.");
      goto do_ps2pk;
    }
    while ((ret=ffgets (rbuff, LBUF, fr)) != FFILE_END) {
      if(ret == BBUFF_FUL) {
        fprintf(stderr, "A line in ps2pk.map seems to be too long.\n");
        fprintf(stderr, "I try to continue. But something may be wrong.\n");
      }
      p = rbuff;
      skipchar(&p);
      if((*p == '%') || (*p == '#') || (*p == '\n'))
        continue;
      q = texname;
      while(!isskip(*p) && (*p != '\n'))
        *q++ = *p++;
      *q = '\0';
      if(stricmp(texname, name))
        continue;
      skipchar(&p);
      if((*p == '%') || (*p == '#') || (*p == '\n')) {
        fprintf(stderr, "Incorrect line in \"ps2pk.map\".\n");
        break;
      }
      if(FileName)
        q = a;
      else
        q = psname;
      while(!isskip(*p) && (*p != '\n'))
        *q++ = *p++;
      *q = '\0';
      skipchar(&p);
/*
skip flag
*/
      if(!FileName) {
        while(isdigit(*p))
          p++;
        skipchar(&p);
      }
      if((*p == '%') || (*p == '#') || (*p == '\n')) {
        tpkerr("I cannot use ps2pk due to lack of data.");
        break;
      }
      if(*p == '\"') {
        q = pscommand;
        *q++ = *p++;
        while(*p != '\"')
          *q++ = *p++;
        *q++ = *p++;
        *q = '\0';
        skipchar(&p);
        if((*p == '%') || (*p == '#') || (*p == '\n'))
          break;
      }
      if(FileName && a[0] == '\0')
        q = a;
      else if(FileName && b[0] == '\0')
        q = b;
      else {
        tpkerr("Incorrect line in ps2pk.map.");
        break;
      }
      while(!isskip(*p) && (*p != '\n'))
        *q++ = *p++;
      *q = '\0';
      skipchar(&p);
      if((*p == '%') || (*p == '#') || (*p == '\n'))
        break;
      if(*p == '\"') {
        q = pscommand;
        *q++ = *p++;
        while(*p != '\"')
          *q++ = *p++;
        *q++ = *p++;
        *q = '\0';
        skipchar(&p);
        if((*p == '%') || (*p == '#') || (*p == '\n'))
          break;
      }
      if (FileName && a[0] == '\0')
        q = a;
      else if (FileName && b[0] == '\0')
        q = b;
      else {
        fprintf(stderr, "Incorrect line in \"ps2pk.map\".\n");
        break;
      }
      while(!isskip(*p) && (*p != '\n'))
        *q++ = *p++;
      *q = '\0';
      skipchar(&p);
      if((*p == '%') || (*p == '#') || (*p == '\n'))
        break;
      if(*p == '\"') {
        q = pscommand;
        *q++ = *p++;
        while(*p != '\"')
          *q++ = *p++;
        *q++ = *p++;
        *q = '\0';
        skipchar(&p);
        if((*p == '%') || (*p == '#') || (*p == '\n'))
          break;
      }
      skipchar(&p);
      if((*p == '%') || (*p == '#') || (*p == '\n'))
        break;
      else {
        fprintf(stderr, "Incorrect line in \"ps2pk.map\".\n");
        break;
      }
    }
    fclose(fr);

    if(pscommand[0]) {
      p = strstr(pscommand, "SlantFont");
      if(p) {
        p--;
        while(*p == ' ' || *p == '\t') p--;
        while(*p != ' ' && *p != '\t' && *p != '\"') p--;
        p++;
        sscanf(p, "%lf SlantFont", &slantval);
        sprintf(slant, "%lf", slantval);
        p = slant + strlen(slant) - 1;
        while(*p == '0') {
          *p = '\0';
          p--;
        }
      }
      p = strstr(pscommand, "ExtendFont");
      if(p) {
        p--;
        while(*p == ' ' || *p == '\t') p--;
        while(*p != ' ' && *p != '\t' && *p != '\"') p--;
        p++;
        sscanf(p, "%lf ExtendFont", &extendval);
        sprintf(extend, "%lf", extendval);
        p = extend + strlen(extend) - 1;
        while(*p == '0') {
          *p = '\0';
          p--;
        }
      }
    }
    if(a[0]) {
      p = strrchr(a, '.');
      if(p && !stricmp(p, ".enc")) {
        *p = '\0';
        strcpy(encname, a);
      }
      else if(p && !stricmp(p, ".pfb")) {
        *p = '\0';
        strcpy(pfbname, a);
      }
    }
    if(b[0]) {
      p = strrchr(b, '.');
      if(p && !stricmp(p, ".enc")) {
        *p = '\0';
        strcpy(encname, b);
      }
      else if(p && !stricmp(p, ".pfb")) {
        *p = '\0';
        strcpy(pfbname, b);
      }
    }
    if(pfbname[0] == '\0')
      goto do_ps2pk;
    p = kpse_find_file (pfbname, kpse_type1_format, 0);
    if(!p)
      goto do_ps2pk;
    free(p);
    ps2pkok = 1;
    if(encname[0] && bdpi[0]) {
      i = 5;
    } else if(!encname[0] && !bdpi[0]) {
      i = 3;
    } else {
      i = 4;
    }
  }

 do_ps2pk:

  if (ps2pkok) {
    if (UseAspectRatio) {
      sscanf (dpi, "%lf", &Xdpi);
      Ydpi = Xdpi * AspectRatio;
      sprintf (ydpi, "%d", (int) Ydpi);
    } else
      strcpy (ydpi, dpi);

    strcpy(tfname, name);
    strcat(tfname, ".tfm");
    get_designsize(tfname, designsize);

    if (i == 3) {
      sprintf (cmd, "-X%s -Y%s -S%s -E%s -P%s %s %s",
               dpi, ydpi, slant, extend, designsize, pfbname, pkname);
    } else if (i == 4 && bdpi[0]) {
      sprintf (cmd, "-X%s -Y%s -R%s -S%s -E%s -P%s %s %s",
               dpi, ydpi, bdpi, slant, extend, designsize, pfbname, pkname);
    } else if (i == 4 && encname[0]) {
      sprintf (cmd, "-e%s -X%s -Y%s -S%s -E%s -P%s %s %s",
               encname, dpi, ydpi, slant, extend, designsize, pfbname, pkname);
    } else if (i == 5) {
      sprintf (cmd, "-e%s -X%s -Y%s -R%s -S%s -E%s -P%s %s %s",
               encname, dpi, ydpi, bdpi, slant, extend, designsize, pfbname, pkname);
    } else {
      tpkerr ("File format of pspksupp.map is wrong.");
      goto do_gsftopk;
    }

    strcpy (execfile, "ps2pk.exe");
    fprintf (stderr, "%s %s\n", execfile, cmd);
    strcpy(fullbin, texbindir);
    strcat(fullbin, execfile);
    (void) _spawnlp (_P_WAIT, fullbin, execfile, cmd, NULL);

    if (_access (pkname, 0) != 0) {
      tpkerr ("ps2pk failed to make pk font.");
      goto do_gsftopk;
    }
    goto finale;
  }

/*
 * ps2pk is impossible to use
 */

 do_gsftopk:

  tpkerr ("ps2pk cannot be used.");
  tpkerr ("I try gsftopk.");
  app = 2;

  strcpy (execfile, "gsftopk.exe");
  fprintf (stderr, "%s %s %s\n", execfile, name, dpi);
  strcpy(fullbin, texbindir);
  strcat(fullbin, execfile);
  (void) _spawnlp (_P_WAIT, fullbin, execfile, name, dpi, NULL);

  if (_access (pkname, 0) != 0) {
    tpkerr ("gsftopk cannot be used.");
    tpkerr ("Next I try ttf2pk.");
    app = 3;
    strcpy (execfile, "ttf2pk.exe");
    fprintf (stderr, "%s -q %s %s\n", execfile, name, dpi);
    strcpy(fullbin, texbindir);
    strcat(fullbin, execfile);
    (void) _spawnlp (_P_WAIT, fullbin, execfile, "-q", name, dpi, NULL);

    if (_access (pkname, 0) != 0) {
      tpkerr ("ttf2pk failed.");
      tpkerr ("Finally I try hbf2gf.");
      app = 4;
      strcpy (execfile, "hbf2gf.exe");
      fprintf (stderr, "%s -q -p %s %s\n", execfile, name, dpi);
      strcpy(fullbin, texbindir);
      strcat(fullbin, execfile);
      (void) _spawnlp (_P_WAIT, fullbin, execfile, "-q -p", name, dpi, NULL);

      sprintf (cmd, "%s.%sgf", name, dpi);
      if (_access (cmd, 0) != 0) {
        tpkerr ("All trials failed.");
        _chdrive (cdrive);
        _chdir (currdir);
        relmem (arg);
        free(tmp);
        return (100);
      }
      strcpy (execfile, "gftopk.exe");
      fprintf (stderr, "%s %s %s\n", execfile, cmd, pkname);
      strcpy(fullbin, texbindir);
      strcat(fullbin, execfile);
      (void) _spawnlp (_P_WAIT, fullbin, execfile, cmd, pkname, NULL);

      if (_access (pkname, 0) != 0) {
        tpkerr ("All trials failed.");
        _chdrive (cdrive);
        _chdir (currdir);
        relmem (arg);
        free(tmp);
        return (100);
      }
      remove (cmd);
    }
  }

 finale:

/*
 * return to original stdout and stdin
 */
  _dup2 (savo, fileno (stdout));
  close (savo);
  _dup2 (savi, fileno (stdin));
  close (savi);

/*
 * close nul device
 */
  fclose (fnul);

/*
 * copy the pk file
 */
  if (!(fr = fopen (pkname, "rb"))) {
    fprintf (stderr, "Cannot open %s to read.\n", pkname);
    _chdrive (cdrive);
    _chdir (currdir);
    relmem (arg);
    free(tmp);
    return (100);
  }

  if (!(fw = fopen (buff, "wb"))) {
    fprintf (stderr, "Cannot open %s to write.\n", buff);
    _chdrive (cdrive);
    _chdir (currdir);
    relmem (arg);
    free(tmp);
    return (100);
  }

  while ((i = (int)fread (rbuff, 1, LBUF, fr)))
    fwrite (rbuff, 1, i, fw);

  fclose (fr);
  fclose (fw);
  remove (pkname);

  relmem (arg);

/*
 * update ls-R if it exists
 */
  mktexupd (buff);

/*
 * tell kpathsea
 */

  printf ("%s\n", buff);
  _chdrive (cdrive);
  _chdir (currdir);
  free(tmp);

  return (0);
}
