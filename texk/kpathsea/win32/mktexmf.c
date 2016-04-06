/* mktexmf.c

   Copyright 2000, 2016 Akira Kakuto.

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

#include <kpathsea/kpathsea.h>
#include "mktex.h"

#define SBUF 512

static int
test_file(char c, char * name)
{
/*
 * c= 'z', 'f', 'd', 'r'
 *     
*/
  FILE *fp;

  if (c == 'z') {
    if (name == NULL) return 1;
    else if (*name == '\0') return 1;
    else if (_access(name, 0) != 0) return 1;
    else if ((fp = (FILE *)fopen(name, "r")) == NULL) return 1;
    else if (_filelength(_fileno(fp)) == 0L) {
      fclose(fp);
      return 1;
    }
    else {
      fclose(fp);
      return 0;
    }
  }
  else if (c == 'f') {
    if (name == NULL) return 0;
    else if (*name == '\0') return 0;
    else if (_access(name, 0) != 0) return 0;
    else return 1;
  }
  else if (c == 'd') {
    if (is_dir(name)) return 1;
    else return 0;
  }
  else if (c == 'r') {
    if (_access(name, 4) == 0) return 1;
    else return 0;
  }
  /* never reaches here */
  return 0;
}

static void
usage(void)
{
  fprintf(stderr, "Usage : mktexmf FONT.\n\n");
  fprintf(stderr, "Makes the Metafont source file for FONT,"
          " if possible. For example,\n");
  fprintf(stderr, "`ecr12' or `cmr11'.\n");
}

/*
 * Split a .mf name into root part and point size part.
 * Root and point size are optional (may be NULL).
 */

static void
split_mf_name(string name, string *base, string *root, string *ptsize)
{
  string p, q;
  /* name = basename $1 .mf */
  p = strrchr (name, '.');
  if (p) {
    if (stricmp (p, ".mf") == 0) *p = '\0';
  }
  p = name;

  if (base)
    *base = xstrdup(p);
  /* rootname = `echo $name | sed 's/[0-9]*$//'` */
  for (q = p + strlen(p);  q > p && isdigit(q[-1]); q--);
  /* ptsize = `echo $name | sed 's/^$rootname//'` */
  if (ptsize)
    *ptsize = xstrdup(q);
  *q = '\0';
  if (root)
    *root = p;
  else
    free(p);
}

static void
relmem (char **v)
{
  int j;
  for (j = 0; j < 4; j++)
    free (v[j]);
  return;
}

int
main (int argc, char **argv)
{
  string rootname, pointsize;
  const_string realsize;
  string name;
  string sauterroot, rootfile;
  string destdir = NULL;
  FILE *f;
  size_t ptsz_len;
  char font[SBUF];
  char *progname;
  char mfname[SBUF];
  char *arg[4];
  int  i;
  char *ptr;

  kpse_set_program_name (argv[0], NULL);
  progname = kpse_program_name;

  if (argc != 2) {
    usage();
    return 0;
  }

  if (strncmp(argv[1], "-v",2) == 0 || strncmp(argv[1], "--v",3) == 0) {
    fprintf ( stderr, "%s, (C version 1.1 --ak 2006-2015)\n", progname);
    return 0;
  }

  if (strncmp(argv[1], "-h",2) == 0 || strncmp(argv[1], "--h",3) == 0) {
    fprintf ( stderr, "%s, Usage: %s FontBaseName\n", progname, progname);
    return 0;
  }

  for (i=0; i < 4; i++) {
    arg[i] = (char *)malloc(SBUF);
  }

  if(strlen(argv[1]) > SBUF - 1) {
    fprintf(stderr, "\nToo long a font name.\n");
    return 100;
  }

  strcpy (font, argv[1]);

  split_mf_name(font, &name, &rootname, &pointsize);


  fprintf(stderr, "name = %s, rootname = %s, pointsize = %s\n",
          name, rootname, pointsize);      


  sauterroot = kpse_find_file(concat3("b-", rootname, ".mf"),
                              kpse_mf_format, false);

  if (sauterroot && *sauterroot) {
    rootfile = sauterroot;
    rootname = concat("b-", rootname);
    strcpy (arg[0], "Dummy");
    strcpy (arg[1], "source");
    strcpy (arg[2], sauterroot);
    if (!(ptr = getdestdir (3, arg))) {
      fprintf(stderr, "Cannot get destination directory name.\n");
      relmem (arg);
      return 100;
    }
    destdir = ptr;
  }
  else if (strlen(name) == 8
           && FILESTRNCASEEQ(name, "csso12", 6)
           && (name[6] >= '0' && name[6] <= '5')
           && isdigit(name[7])) {
    rootfile = xstrdup("");
  }
  else if (FILESTRNCASEEQ(rootname, "cs", 2)
           || FILESTRNCASEEQ(rootname, "lcsss", 5)
           || FILESTRNCASEEQ(rootname, "icscsc", 6)
           || FILESTRNCASEEQ(rootname, "icstt", 5)
           || FILESTRNCASEEQ(rootname, "ilcsss", 6)
           ) {
    rootfile = kpse_find_file("cscode.mf",
                              kpse_mf_format, false);
  }
  else if (strlen(rootname) >= 3
           && ((FILESTRNCASEEQ(rootname, "wn", 2)
                && strchr("bBcCdDfFiIrRsStTuUvV", rootname[2]))
               || (FILESTRNCASEEQ(rootname, "rx", 2)
                   && strchr("bBcCdDfFiIoOrRsStTuUvVxX", rootname[2])
                   && strlen(rootname) >= 4
                   && strchr("bBcCfFhHiIlLmMoOsStTxX", rootname[3]))
               || ((rootname[0] == 'l' || rootname[0] == 'L')
                   && strchr("aAbBcCdDhHlL", rootname[1])
                   && strchr("bBcCdDfFiIoOrRsStTuUvVxX", rootname[2])))) {
    char lhprefix[64];
    strncpy(lhprefix, name, 2);
    lhprefix[2] = '\0';
    strcat(lhprefix, "codes.mf");
    rootfile = kpse_find_file(lhprefix, kpse_mf_format, false);
  }
  else {
    string tem;
    rootfile = kpse_find_file(tem = concat(rootname, ".mf"),
                              kpse_mf_format, false);
    free(tem);
  }

  if (test_file('z', rootfile)) {
    fprintf (stderr, "%s: empty or non-existent rootfile!\n", progname);
    relmem (arg);
    return 1;
  }
  if (!test_file('f', rootfile)) {
    fprintf (stderr, "%s: rootfile %s does not exist!\n", progname, rootfile);
    relmem (arg);
    return 1;
  }

  if (!destdir) {
    if (rootfile && *rootfile) {
      strcpy (arg[0], "Dummy");
      strcpy (arg[1], "source");
      strcpy (arg[2], rootfile);
      if (!(ptr = getdestdir (3, arg))) {
        fprintf(stderr, "Cannot get destination directory name.\n");
        relmem (arg);
        return 1;
      }
      destdir = ptr;
    }
  }

  if (!test_file('d', destdir)) {
    relmem (arg);
    return 1;
  }

  ptsz_len = strlen(pointsize);
  if (ptsz_len == 0) {
    fprintf(stderr, "%s: no pointsize.\n", progname);
    relmem (arg);
    return 1;
  } else if (ptsz_len == 2) {
    if (pointsize[0] == '1' && pointsize[1] == '1')
      realsize = "10.95";               /* \magstephalf */
    else if (pointsize[0] == '1' && pointsize[1] == '4')
      realsize = "14.4";                /* \magstep2 */
    else if (pointsize[0] == '1' && pointsize[1] == '7')
      realsize = "17.28";               /* \magstep3 */
    else if (pointsize[0] == '2' && pointsize[1] == '0')
      realsize = "20.74";               /* \magstep4 */
    else if (pointsize[0] == '2' && pointsize[1] == '5')
      realsize = "24.88";               /* \magstep5 */
    else if (pointsize[0] == '3' && pointsize[1] == '0')
      realsize = "29.86";               /* \magstep6 */
    else if (pointsize[0] == '3' && pointsize[1] == '6')
      realsize = "35.83";               /* \magstep7 */
    else
      realsize = pointsize;
  }
  /* The new convention is to have three or four letters for the
     font name and four digits for the pointsize. The number is
     pointsize * 100. We effectively divide by 100 by inserting a
     dot before the last two digits.  */
  else if (ptsz_len == 4 || ptsz_len == 5) {
    /* realsize=`echo "$pointsize" | sed 's/\(..\)$/.\1/'` */
    string tempsize = (string)xmalloc(ptsz_len + 2);
    strcpy(tempsize, pointsize);
    /* The script doesn't check for last chars being digits, but we do!  */
    if (isdigit(tempsize[ptsz_len-1])
        && isdigit(tempsize[ptsz_len-2])) {
      tempsize[ptsz_len+1] = '\0';
      tempsize[ptsz_len]   = tempsize[ptsz_len-1];
      tempsize[ptsz_len-1] = tempsize[ptsz_len-2];
      tempsize[ptsz_len-2] = '.';
      free(pointsize);
    }
    realsize = tempsize;
  } else realsize = pointsize;

/* mfname is the full name */
  strcpy (mfname, destdir);
  i = (int)strlen (mfname);
  if (mfname[i-1] != '/') {
    mfname[i] = '/';
    mfname[i+1] = '\0';
  }

  strcat (mfname, name);
  strcat (mfname, ".mf");

  if (test_file('r', mfname)) {
    fprintf(stderr, "%s: %s already exists.\n", progname, mfname);
    fprintf(stdout, "%s\n", mfname);
    if (destdir) free (destdir);
    relmem (arg);
    return 0;
  }

  if ((f = fopen(mfname, "wb")) == NULL) {
    fprintf(stderr, "%s: can't write into the file %s/%s.\n",
            progname, destdir, name);
    if (destdir) free (destdir);
    relmem (arg);
    return 1;
  }

  if (FILESTRNCASEEQ(name, "ec", 2)
      || FILESTRNCASEEQ(name, "tc", 2)) {
    fprintf(f, "if unknown exbase: input exbase fi;\n");
    fprintf(f, "gensize:=%s;\ngenerate %s;\n", realsize, rootname);
  }
  else if (FILESTRNCASEEQ(name, "dc", 2)) {
    fprintf(f, "if unknown dxbase: input dxbase fi;\n");
    fprintf(f, "gensize:=%s;\ngenerate %s;\n", realsize, rootname);

  }
  else if (FILESTRNCASEEQ(name, "cs", 2)
           || FILESTRNCASEEQ(name, "lcsss", 5)
           || FILESTRNCASEEQ(name, "icscsc", 6)
           || FILESTRNCASEEQ(name, "icstt", 5)
           || FILESTRNCASEEQ(name, "ilcsss", 6)
           ) {
    fprintf(f, "input cscode\nuse_driver;\n");
  }
  else if (strlen(name) >= 3
           && ((FILESTRNCASEEQ(name, "wn", 2)
                && strchr("bBcCdDfFiIrRsStTuUvV", name[2]))
               || (FILESTRNCASEEQ(name, "rx", 2)
                   && strchr("bBcCdDfFiIoOrRsStTuUvVxX", name[2])
                   && strlen(name) >= 4
                   && strchr("bBcCfFhHiIlLmMoOsStTxX", name[3]))
               || ((name[0] == 'l' || name[0] == 'L')
                   && strchr("aAbBcCdDhHlL", name[1])
                   && strchr("bBcCdDfFiIoOrRsStTuUvVxX", name[2])))) {
    fprintf(f, "input fikparm;\n");
  }
  else if (strlen(name) >= 4 && strchr("gG", name[0])
           && strchr("lLmMoOrRsStT", name[1]) 
           && strchr("bBiIjJmMtTwWxX", name[2]) 
           && strchr("cCiIlLnNoOrRuU", name[3])) {
    /* A small superset of the names of the cbgreek fonts:
       pattern `g[lmorst][bijmtwx][cilnou]*'. 
       This is only slightly more general than the exact set of patterns.
     */
    fprintf(f, "gensize:=%s;\ninput %s;\n", realsize, rootname);
  }
  else {
    /* FIXME: this was in the previous versions */
    /* fprintf(f, "if unknown %s: input %s fi;\n", base, base); */
      fprintf(f, "design_size := %s;\ninput %s;\n",
              realsize, rootname);
  }

  fclose(f);
  fprintf(stdout, "%s\n", mfname);
  fprintf(stderr, "%s: %s: successfully generated.\n", progname, mfname);
  mktexupd (mfname);
  if (destdir) free (destdir);
  relmem (arg);
  return 0;
}
