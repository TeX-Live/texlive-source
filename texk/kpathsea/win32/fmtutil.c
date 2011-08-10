/*

Try TEXMFVAR as a head directory, if not defined try TEXMFMAIN next.


fmtutil.c   (2002/10/06 --ak)
Web2C-7.3.9 (2002/10/25 --ak)
Web2C-7.4.2 (2002/12/27 --ak)
Web2C-7.4.3 (2003/01/12 --ak)
Web2C-7.5.2 (2003/11/09 --ak)
Web2C-7.5.3 (2004/05/09 --ak)
Web2C-7.5.3 (2004/06/11 --ak)
Web2C-7.5.5 (2005/12/23 --ak)
Web2C-7.5.5 (2006/11/25 --ak)
Web2C-7.5.6 (2007/06/01 --ak) save log files.
Web2C-7.5.6 (2008/01/20 --ak) Create necessary directories.
Web2C-2009  (2009/06/11 --ak) error handling.
Web2C-2009  (2009/08/13 Nobert) new option --byengine
Web2C-2009  (2009/10/10 --ak) creates mplib-luatex.mem.
Web2C-2010  (2010/05/31 --ak) multiple language files.
Web2C-2010  (2010/07/03 --ak) new options.
*/

#include <kpathsea/kpathsea.h>

#include "dirutil.h"
#include "mktexupd.h"

#ifndef __MINGW32__
#undef system
#define system win32_system
#endif

#define BLEN       512
#define SLEN       256
#define MAXFMT     128
#define SIXTY4     64
#define SIXTEEN    16
#define DEFAULTCNF "fmtutil.cnf"
#define REVISION   "fmtutil (C version 0.7) 2002-2010 --ak"

typedef struct {
char *format;
char *engine;
char *hyphen;
char *args;
} cmd_t;

/*
Global variables
*/
cmd_t K[MAXFMT];
int   Fmtnum = 0;
char  Buff[BLEN];
char  Programname[BLEN];
char  Enginename[BLEN];
char  Realenginename[BLEN];
char  Destdir[BLEN];
char  Currdir[BLEN];
char  *Exefile[MAXFMT];
unsigned char  Exenum = 0;
int   Errorcnt = 0;
int   UserDestdir = 0;
int   QQQ = 0; /* quiet */

static void
revision(void)
{
  printf(REVISION "\n");
}

static void
usage(void)
{
  printf(
"Commands:\n"
" --all                     recreate all format, base, mem files\n"
" --missing                 create all missing format, base, mem files\n"
" --refresh                 recreate existing format, base, mem files\n"
" --byfmt FORMATNAME        (re)create format for `FORMATNAME'\n"
"                           `base' and `mem' files can also be created\n"
" --byhyphen HYPHENFILE     (re)create formats that depend on `HYPHENFILE'\n"
" --byengine ENGINENAME     (re)create formats that depend on `ENGINENAME'\n"
" --showhyphen FORMATNAME   print name of hyphenfile for format `FORMATNAME'\n"
" --version                 show revision number\n"
" --help                    show this message\n"
" --enablefmt FORMATNAME    (not implemented)\n"
" --disablefmt FORMATNAME   (not implemented)\n"
" --listcfg                 (not implemented)\n"
" --catcfg                  (not implemented)\n"
" --edit                    (not implemented)\n"
"\n"
"Options:\n"
" --cnffile FILE            set configfile for fmtutil\n"
"                           default is `fmtutil.cnf'\n"
" --fmtdir DIRECTORY        set destination directory for format files\n"
"                           `DIRECTORY' must be an absolute path\n"
"                           default is $TEXMFVAR/web2c/$engine\n"
"                           or $TEXMFMAIN/web2c/$engine if TEXMFVAR is undefined\n"
" --engine TEXENGINE        specify the TeX engine (with --byfmt fmtname)\n"
" --quiet                   be silent\n"
" --no-error-if-no-format   (just for compatibility)\n"
" --no-engine-subdir        (not implemented)\n"
" --test                    (not implemented)\n"
" --dolinks                 (not implemented)\n"
" --force                   (not implemented)\n");
}

static void
freeK(void)
{
  int i;
  for(i=0; i < MAXFMT; i++) {
    free(K[i].format);
    free(K[i].engine);
    free(K[i].hyphen);
    free(K[i].args);
    free(Exefile[i]);
  }
}

static int
do_initex(char *fm, char *en, char *args)
{
  int  ret, lsrflag;
  char fmt[BLEN];
  char ext[SIXTEEN];
  char cmd[BLEN];
  char progn[BLEN];
  char progn2[BLEN];
  char enginesubdir[BLEN];
  char src[BLEN];
  char dst[BLEN];
  char logdst[BLEN];
  char log[BLEN];
  char fullbin[256], *pp;

  if(!strnicmp(fm, "cont", 4)) strcpy(progn, "context");
  else if(!strnicmp(fm, "ptex-", 5)) strcpy(progn, "ptex");
  else if(!strnicmp(fm, "platex-", 7)) strcpy(progn, "platex");
  else strcpy(progn, fm);

  strcpy(enginesubdir, en);

  if(!stricmp(enginesubdir, "mf-nowin"))
    strcpy(enginesubdir, "metafont");
  if(!stricmp(enginesubdir, "mf"))
    strcpy(enginesubdir, "metafont");
  if(!stricmp(enginesubdir, "mpost"))
    strcpy(enginesubdir, "metapost");

  if(!stricmp(en, "mf-nowin")) {
    strcpy(fmt, "--jobname=");
    strcpy(ext, ".base");
  }
  else if(!stricmp(en, "mf")) {
    strcpy(fmt, "--jobname=");
    strcpy(ext, ".base");
  }
  else if(!stricmp(en, "mpost")) {
    strcpy(fmt, "--jobname=");
    strcpy(ext, ".mem");
  }
  else if(!stricmp(en, "pmpost")) {
    strcpy(fmt, "--jobname=");
    strcpy(ext, ".mem");
  }
  else if(!stricmp(en, "upmpost")) {
    strcpy(fmt, "--jobname=");
    strcpy(ext, ".mem");
  }
  else if(!stricmp(en, "mp641")) {
    strcpy(fmt, "--jobname=");
    strcpy(ext, ".mem");
  }
  else if(!stricmp(en, "nts")) {
    strcpy(fmt, "--fmt=");
    strcpy(ext, ".nfmt");
  }
  else {
    strcpy(fmt, "--jobname=");
    strcpy(ext, ".fmt");
  }

  if(!stricmp(en, "pmpost") || !stricmp(en, "mpost")
     || !stricmp(en, "upmpost") || !stricmp(en, "mp641"))
    strcpy(progn2, "mpost");
  else if(!stricmp(en, "mf-nowin"))
    strcpy(progn2, "mf");
  else if(!stricmp(en, "nts"))
    strcpy(progn2, "tex");
  else strcpy(progn2, progn);

  pp = kpse_var_value("SELFAUTOLOC");
  if(pp == NULL) {
     fprintf(stderr, "I cannot get SELFAUTOLOC\n");
     exit(100);
  }
  fullbin[0] = '\"';
  fullbin[1] = '\0';
  strcat(fullbin, pp);
  free(pp);
  for(pp=fullbin; *pp; pp++) {
     if(*pp == '/') *pp = '\\';
  }
  strcat(fullbin, "\\");
  strcat(fullbin, en);
  strcat(fullbin, "\"");

  if(!stricmp(en, "nts"))
    sprintf(cmd, "%s --ini --progname=%s %s <nul",
            fullbin, progn2, args);
  else
    sprintf(cmd, "%s --ini %s%s --progname=%s %s <nul",
            fullbin, fmt, fm, progn2, args);

  if(QQQ == 0)
    fprintf(stderr, "Running: %s\n", cmd);
  ret = system(cmd);
  strcpy(src, fm);
  strcpy(log, fm);
  strcat(log, ".log");
  strcat(src, ext);
  strcpy(dst, Destdir);

  if((UserDestdir == 0) && stricmp(enginesubdir, "nts")) {
    strcat(dst, enginesubdir);
    if(!is_dir(dst)) {
      if(make_dir(dst)) {
        fprintf(stderr, "Failed to make format in %s.\n", dst);
        return (-100);
      }
    }
    strcat(dst, "/");
  }

  strcpy(logdst, dst);
  strcat(logdst, log);
  strcat(dst, src);
  if(_access(dst, 0) == 0)
    lsrflag = 0;
  else
    lsrflag = 1;

  if((_access(src, 0) == 0) && ret == 0) {
    CopyFile(src, dst, FALSE);
    if(lsrflag) mktexupd(dst);
    if(stricmp(src, "mpost.mem") == 0) {
       int len;
       char *tmpstr;
       len = strlen(dst);
       tmpstr = (char *)malloc(len + 10);
       strcpy(tmpstr, dst);
       tmpstr[len-9] = '\0';
       strcat(tmpstr, "mplib-luatex.mem");
       if(_access(tmpstr, 0) == 0)
          free(tmpstr);
       else {
         CopyFile(src, tmpstr, FALSE);
         mktexupd(tmpstr);
         free(tmpstr);
      }
    }
    remove(src);
    if(_access(logdst, 0) == 0)
       lsrflag = 0;
    else
       lsrflag = 1;
    if(_access(log, 0) == 0) {
       CopyFile(log, logdst, FALSE);
       if(lsrflag) mktexupd(logdst);
       remove(log);
    }
    if(QQQ == 0)
      fprintf(stderr, "The file %s%s was created.\n", fm, ext);
    if(!strcmp(progn, "context"))
      strcpy(Exefile[Exenum], "texexec");
    else if(!strcmp(en, "nts"))
      strcpy(Exefile[Exenum], "nts");
    else
      strcpy(Exefile[Exenum], fm);

    Exenum++;
  } else {
    if(_access(src, 0) == 0)
       remove(src);
    if(_access(logdst, 0) == 0)
       lsrflag = 0;
    else
       lsrflag = 1;
    if(_access(log, 0) == 0) {
       CopyFile(log, logdst, FALSE);
       if(lsrflag) mktexupd(logdst);
       remove(log);
    }
    if(QQQ == 0) {
       fprintf(stderr, "\nThe file %s%s was not created due to some errors.\n",
               fm, ext);
       fprintf(stderr, "See the log file %s for details.\n", logdst);
    }
    Errorcnt++; 
    ret = -100;
  }
  return ret;
}

static void
do_all(void)
{
  int i;

  for(i = 0; i < Fmtnum; i++)
    do_initex(K[i].format, K[i].engine, K[i].args);
}

static void
show_hyphen(char *fm, char *hfile)
{
  char *p, *q, *r;
  int  cont;
  char progn[BLEN];

  if(!strcmp(hfile, "-")) {
    printf("%s\n", hfile);
    return;
  }

  if(!strnicmp(fm, "cont", 4)) strcpy(progn, "context");
  else strcpy(progn, fm);
  if(QQQ == 0)
    fprintf(stderr, "Searching ... \n");
  kpse_reset_program_name(progn);
  q = hfile;
  cont = 1;
  while(cont) {
    for(r=q; *r && *r != ','; r++)
      ;
    if(*r == '\0') cont = 0;
    *r = '\0';
    r++;
    p = kpse_find_file(q, kpse_tex_format, 0);
    if(!p) {
      if(QQQ == 0)
        fprintf(stderr, "%s not found.\n", q);
      q = r;
      continue;
    }
    printf("%s\n", p);
    free(p);
    q = r;
  }
  kpse_reset_program_name(Programname);
  return;
}

static int
testexe(char *name, const char *suffix)
{
  char buffer[BLEN];
  char *p;

  if(SearchPath(NULL, name, suffix, BLEN, buffer, &p))
    return 1;
  else
    return 0;
}

int
main(int ac, char **av)
{
  FILE *in;

  int  i, j, k;
  char *cnfname;
  char *p;
  char cnffile[BLEN];
  char fmtname[BLEN];
  char hyphenfile[BLEN];
  char byenginename[BLEN];
  char kpsedot[BLEN];
  char enginesubdir[BLEN];
  char *tmp;
  int  cdrive, tdrive;

  int  flag;
  
/*
flag = 1 : --all
       2 : --missing
       3 : --byfmt formatname
       4 : --byhyphen hyphenfile
       5 : --showhyphen formatname
       6 : --refresh
       7 : --byengine engine
*/

  flag = 0;
  cnffile[0] = '\0';
  fmtname[0] = '\0';
  hyphenfile[0] = '\0';
  byenginename[0] = '\0';
  Destdir[0] = '\0';
  Currdir[0] = '\0';
  Enginename[0] = '\0';
  Realenginename[0] = '\0';

  kpse_set_program_name(av[0], NULL);
  strcpy(Programname, av[0]);

  tmp = getenv("TEMP");
  if(!tmp) tmp = getenv("TMP");
  if(!tmp) tmp = getenv("TMPDIR");
  if(!tmp) {
    fprintf(stderr, "Please define TEMP | TMP | TMPDIR.\n");
    return (100);
  }

  tmp = xstrdup(tmp);
  for(p = tmp; *p; p++) {
    if(*p == '\\')
      *p = '/';
    else if (IS_KANJI(p))
      p++;
  }

  i = strlen(tmp);
  if(tmp[i-1] == '/') tmp[i-1] = '\0';

  for(i = 1; i < ac; i++) {
    if(strlen(av[i]) > BLEN-1) {
      fprintf(stderr, "\nToo long a string.\n");
      return 100;
    }
    /* --cnffile FILE */

    if(!strncmp(av[i],"--cn", 4) || !strncmp(av[i], "-cn", 3)) {
      i++;
      if(i >= ac) {
        fprintf(stderr, "Invalid argument.\n");
        exit (100);
      }
      strcpy(cnffile, av[i]);
      continue;
    }
    /* --engine  TEXENGINE */

    else if(!strncmp(av[i],"--eng", 5) || !strncmp(av[i], "-eng", 4)) {
      i++;
      if(i >= ac) {
        fprintf(stderr, "Invalid argument.\n");
        exit (100);
      }
      strcpy(Enginename, av[i]);
      continue;
    }
    /* --fmtdir  DIRCTORY */

    else if(!strncmp(av[i],"--fm", 4) || !strncmp(av[i], "-fm", 3)) {
      i++;
      if(i >= ac) {
        fprintf(stderr, "Invalid argument.\n");
        exit (100);
      }
      strcpy(Destdir, av[i]);
      UserDestdir = 1;
      continue;
    }
    /* --all */

    else if(!strncmp(av[i],"--a", 3) || !strncmp(av[i], "-a", 2)) {
      flag = 1;
      continue;
    }
    /* --missing */

    else if(!strncmp(av[i],"--m", 3) || !strncmp(av[i], "-m", 2)) {
      flag = 2;
      continue;
    }
    /* --refresh */

    else if(!strncmp(av[i],"--r", 3) || !strncmp(av[i], "-r", 2)) {
      flag = 6;
      continue;
    }
    /* --byfmt FORMATNAME */

    else if(!strncmp(av[i],"--byf", 5) || !strncmp(av[i], "-byf", 4)) {
      i++;
      if(i >= ac) {
        fprintf(stderr, "Invalid argument.\n");
        exit (100);
      }
      flag = 3;
      strcpy(fmtname, av[i]);
      continue;
    }
    /* --byhyphen HYPHENFILE */

    else if(!strncmp(av[i],"--byh", 5) || !strncmp(av[i], "-byh", 4)) {
      i++;
      if(i >= ac) {
        fprintf(stderr, "Invalid argument.\n");
        exit (100);
      }
      flag = 4;
      strcpy(hyphenfile, av[i]);
      for(p=hyphenfile; *p; p++) {
	if(*p == '\\')
	  *p = '/';
        else if (IS_KANJI(p))
          p++;
      }
      p = strrchr(hyphenfile, '/');
      if(p) {
	p++;
	strcpy(hyphenfile, p);
      }
      continue;
    }
    /* --byengine ENGINENAME */

    else if(!strncmp(av[i],"--bye", 5) || !strncmp(av[i], "-bye", 4)) {
      i++;
      if(i >= ac) {
        fprintf(stderr, "Invalid argument.\n");
        exit (100);
      }
      flag = 7;
      strcpy(byenginename, av[i]);
      continue;
    } 
    /* --showhyphen FORMATNAME */

    else if(!strncmp(av[i],"--s", 3) || !strncmp(av[i], "-s", 2)) {
      i++;
      if(i >= ac) {
        fprintf(stderr, "Invalid argument.\n");
        exit (100);
      }
      flag = 5;
      strcpy(fmtname, av[i]);
      continue;
    }
    /* --version */

    else if(!strncmp(av[i],"--v", 3) || !strncmp(av[i], "-v", 2)) {
      revision();
      return 0;
    }
    /* --help */

    else if(!strncmp(av[i],"--h", 3) || !strncmp(av[i], "-h", 2)) {
      usage();
      return 0;
    }
    /* --enablefmt FORMATNAME */

    else if(!strncmp(av[i],"--ena", 5) || !strncmp(av[i], "-ena", 4)) {
      i++;
      if(i >= ac) {
        fprintf(stderr, "Invalid argument.\n");
        exit (100);
      }
      fprintf(stderr, "`--enablefmt FORMATNAME' is not implemented.\n");
      continue;
    }
    /* --disablefmt FORMATNAME */

    else if(!strncmp(av[i],"--di", 4) || !strncmp(av[i], "-di", 3)) {
      i++;
      if(i >= ac) {
        fprintf(stderr, "Invalid argument.\n");
        exit (100);
      }
      fprintf(stderr, "`--disablefmt FORMATNAME' is not implemented.\n");
      continue;
    }
    /* --listcfg */

    else if(!strncmp(av[i],"--l", 3) || !strncmp(av[i], "-l", 2)) {
      fprintf(stderr, "`--listcfg' is not implemented.\n");
      continue;
    }
    /* --catcfg */

    else if(!strncmp(av[i],"--ca", 4) || !strncmp(av[i], "-ca", 3)) {
      fprintf(stderr, "`--catcfg' is not implemented.\n");
      continue;
    }
    /* --edit */

    else if(!strncmp(av[i],"--ed", 4) || !strncmp(av[i], "-ed", 3)) {
      fprintf(stderr, "`--edit' is not implemented.\n");
      continue;
    }
    /* --test */

    else if(!strncmp(av[i],"--t", 3) || !strncmp(av[i], "-t", 2)) {
      fprintf(stderr, "`--test' is not implemented.\n");
      continue;
    }
    /* --dolinks */

    else if(!strncmp(av[i],"--do", 4) || !strncmp(av[i], "-do", 3)) {
      fprintf(stderr, "`--dolinks' is not implemented.\n");
      continue;
    }
    /* --force */

    else if(!strncmp(av[i],"--fo", 4) || !strncmp(av[i], "-fo", 3)) {
      fprintf(stderr, "`--force' is not implemented.\n");
      continue;
    }
    /* --no-engine-subdir */

    else if(!strncmp(av[i],"--no-en", 7) || !strncmp(av[i], "-no-en", 6)) {
      fprintf(stderr, "`--no-engine-subdir' is not implemented.\n");
      continue;
    }
    /* --no-error-if-no-format */

    else if(!strncmp(av[i],"--no-er", 7) || !strncmp(av[i], "-no-er", 6)) {
      continue;
    }
    /* --quiet */

    else if(!strncmp(av[i],"--q", 3) || !strncmp(av[i], "-q", 2)) {
      QQQ = 1;
      continue;
    }
    else {
      fprintf(stderr, "%s: Invalid argument.\n\n", av[0]);
      usage();
      return 100;
    }
  }

  if(flag == 0) {
    usage();
    return 100;
  }


  if(!cnffile[0]) strcpy(cnffile, DEFAULTCNF);
  cnfname = kpse_find_file(cnffile, kpse_web2c_format, 0);
  if(!cnfname) {
    fprintf(stderr, "Failed to find cnffile %s.\n", cnffile);
    exit (100);
  }

  in = fopen(cnfname, "r");
  free(cnfname);
  if(!in) {
    fprintf(stderr, "Failed to open %s.\n", cnffile);
    exit (100);
  }

  for(i=0; i < MAXFMT; i++) {
    K[i].format = (char *)malloc(BLEN);
    K[i].engine = (char *)malloc(BLEN);
    K[i].hyphen = (char *)malloc(BLEN);
    K[i].args = (char *)malloc(BLEN);
    Exefile[i] = (char *)malloc(BLEN);
  }

  i = 0;
  while(fgets(Buff, BLEN, in)) {
    if(Buff[0] == '#' ||
       Buff[0] == '\n' ||
       Buff[0] == 0x25)
      continue;
    p = strtok(Buff, " \t");
    if(p)
      strcpy(K[i].format, p);
    else
      K[i].format[0] = '\0';
    p = strtok(NULL, " \t");
    if(p)
      strcpy(K[i].engine, p);
    else
      K[i].engine[0] = '\0';
    p = strtok(NULL, " \t");
    if(p)
      strcpy(K[i].hyphen, p);
    else
      K[i].hyphen[0] = '\0';
    p = strtok(NULL, "\n");
    if(p) {
      while(*p == ' ' || *p == '\t')
        p++;
      j = strlen(p);
      if(*(p+j-1) == '\n')
        *(p+j-1) = '\0';
      strcpy(K[i].args, p);
    }
    else
      K[i].args[0] = '\0';

    if(!K[i].format[0] ||
       !K[i].engine[0] ||
       !K[i].hyphen[0] ||
       !K[i].args) {
      fprintf(stderr, "Invalid line in %s.\n", cnffile);
      freeK();
      return (100);
    }
    i++;
    if(i > MAXFMT) {
      fprintf(stderr, "Too many lines in %s.\n", cnffile);
      freeK();
      return (100);
    }
  }

  Fmtnum = i;
  fclose(in);

  j = 0;
  if(flag == 5) {
    for(i = 0; i < Fmtnum; i++) {
      if(!stricmp(K[i].format, fmtname)) {
        show_hyphen(K[i].format, K[i].hyphen);
        j++;
      }
    }
    if(j == 0)
      fprintf(stderr, "argument of --showhyphen must be a format name.\n");
    freeK();
    return 1;
  }

  if(stricmp(Programname, "fmtutil-sys") == 0) {
    char *px = kpse_var_value("TEXMFSYSVAR");
    if(px) {
      xputenv("TEXMFVAR", px);
    }
  }

  if(UserDestdir == 0) {
    p = kpse_var_value("TEXMFVAR");
    if(!p)
      p = kpse_var_value("TEXMFMAIN");
    if(!p) {
      fprintf(stderr, "TEXMFMAIN not defined.\n");
      freeK();
      return 100;
    }
    strcpy(Destdir, p);
    free(p);
    for(p = Destdir; *p ; p++) {
      if(*p == '\\')
        *p = '/';
      else if (IS_KANJI(p))
        p++;
    }

    i = strlen(Destdir);
    while (Destdir[i-1] == '/')
      i--;
    Destdir[i] = '\0';
    strcat(Destdir, "/web2c");
  }

  if(!is_dir(Destdir)) {
    if(make_dir_p(Destdir)) {
      fprintf(stderr, "Failed to make a format under %s.\n", Destdir);
      return (-100);
    }
  }

  strcat(Destdir, "/");
  p = _getcwd(Currdir, BLEN);
  if(p) {
    for(p = Currdir; *p ; p++) {
      if(*p == '\\')
        *p = '/';
      else if (IS_KANJI(p))
        p++;
    }
    i = strlen(Currdir);
    if(Currdir[i-1] == '/') Currdir[i-1] = '\0';
    strcpy(kpsedot, "KPSE_DOT=.;");
    strcat(kpsedot, Currdir);
    _putenv(kpsedot);
  }

  cdrive = _getdrive();
  if(tmp[1] == ':') {
    tdrive = tolower(*tmp) -'a' + 1;
    _chdrive(tdrive);
  }
  _chdir(tmp);

  j = 0;

  if(flag == 4) {
    char *p, *q;
    int  cont;
    for(i = 0; i < Fmtnum; i++) {
      p = K[i].hyphen;
      cont = 1;
      while(cont) {
	for(q = p; *q && *q != ','; q++)
	  ;
	if(*q == '\0') cont = 0;
	*q = '\0';
	q++;
	if(!stricmp(p, hyphenfile)) {
	  do_initex(K[i].format, K[i].engine, K[i].args);
	  j++;
	  cont = 0;
	}
	p = q;
      }
    }
    if(j == 0)
      fprintf(stderr, "hyphen pattern %s not written in %s.\n",hyphenfile,cnffile);
  }

  else if(flag == 7) {
    if(stricmp(byenginename, "metafont") == 0)
      strcpy(byenginename, "mf-nowin");
    if(stricmp(byenginename, "metapost") == 0)
      strcpy(byenginename, "mpost");
    for(i = 0; i < Fmtnum; i++) {
      if(!stricmp(K[i].engine, byenginename)) {
        do_initex(K[i].format, K[i].engine, K[i].args);
        j++;
      }
    }
    if(j == 0) {
      fprintf(stderr, "engine name %s not written in %s.\n", byenginename, cnffile);
    }
  }

  else if(flag == 3) {
    for(i = 0; i < Fmtnum; i++) {
      if(Enginename[0]) {
	if(!stricmp(Enginename, "metafont"))
	  strcpy(Realenginename, "mf-nowin");
	else if(!stricmp(Enginename, "metapost"))
	  strcpy(Realenginename, "mpost");
	else
	  strcpy(Realenginename, Enginename);
        if(!stricmp(K[i].format, fmtname) &&
           !stricmp(K[i].engine, Realenginename)) {
          do_initex(K[i].format, K[i].engine, K[i].args);
          j++;
        }
      }
      else {
        if(!stricmp(K[i].format, fmtname)) {
          do_initex(K[i].format, K[i].engine, K[i].args);
          j++;
        }
      }
    }
    if(j == 0) {
      if(Enginename[0])
        fprintf(stderr, "format name %s with engine %s not written in %s.\n",
               fmtname, Realenginename, cnffile);
      else
        fprintf(stderr, "format name %s not written in %s.\n", fmtname, cnffile);
    }
  }

  else if((flag == 2) || (flag == 6)) {
    for(i = 0; i < Fmtnum; i++) {
      strcpy(Buff, Destdir);

      strcpy(enginesubdir, K[i].engine);
      if(!stricmp(enginesubdir, "mf-nowin"))
        strcpy(enginesubdir, "metafont");
      if(!stricmp(enginesubdir, "mf"))
        strcpy(enginesubdir, "metafont");
      if(!stricmp(enginesubdir, "mpost"))
        strcpy(enginesubdir, "metapost");
      if(stricmp(enginesubdir, "nts"))
        strcat(Buff, enginesubdir);
      strcat(Buff, "/");

      strcat(Buff, K[i].format);
      if(!stricmp(K[i].engine, "nts"))
        strcat(Buff, ".nfmt");
      else if(!stricmp(K[i].engine, "mf"))
        strcat(Buff, ".base");
      else if(!stricmp(K[i].engine, "mf-nowin"))
        strcat(Buff, ".base");
      else if(!stricmp(K[i].engine, "mpost"))
        strcat(Buff, ".mem");
      else if(!stricmp(K[i].engine, "pmpost"))
        strcat(Buff, ".mem");
      else if(!stricmp(K[i].engine, "upmpost"))
        strcat(Buff, ".mem");
      else
        strcat(Buff, ".fmt");
      if(flag == 2) {
	if(_access(Buff, 0) != 0)
	  do_initex(K[i].format, K[i].engine, K[i].args);
	else if(QQQ == 0)
	  fprintf(stderr, "%s exists.\n", Buff);
      } else if(flag == 6) {
	if(_access(Buff, 0) == 0)
	  do_initex(K[i].format, K[i].engine, K[i].args);
	else if(QQQ == 0)
	  fprintf(stderr, "%s does not exist.\n", Buff);
      }
    }
  }

  else if(flag == 1)
    do_all();

  _chdrive(cdrive);
  _chdir(Currdir);

/*
Check executable files. (ignore "Batch" or "Script" files (case 'x'))
*/
  if(Exenum && QQQ == 0) {
    fprintf(stderr, "\n\nNow I check executable files...\n");
    fprintf(stderr, "If some executable files do not exist, you must make them\n");
    fprintf(stderr, "by hard link (Windows NT/2000/XP/Vista/7),"
                    " symbolic link (Vista/7)\n"
                    "or copying (Windows 95/98/Me).\n\n");

    for(i = 0; i < Exenum; i++) {
      if(testexe(Exefile[i], ".exe"))
        sprintf(Buff, "OK. %s.exe exists.", Exefile[i]);
      else if(testexe(Exefile[i], ".bat"))
        sprintf(Buff, "OK. %s.bat exists.", Exefile[i]);
      else if(testexe(Exefile[i], ".sh"))
        sprintf(Buff, "OK. %s.sh exists.", Exefile[i]);
      else
        sprintf(Buff, "I cannot find %s.", Exefile[i]);
      j = strlen(Buff);

      if((i+1) % 2) {
        for(k = j; k < 36; k++)
          Buff[k] = ' ';
        Buff[k] = '\0';
        fprintf(stderr, "%s", Buff);
      }
      else {
        fprintf(stderr, "%s", Buff);
        fprintf(stderr, "\n");
      }
    }
    fprintf(stderr, "\n");
  }

  freeK();
  if(Errorcnt && QQQ == 0) {
    fprintf(stderr, "\n\nI could not create %d of format (base/mem) file(s).\n",
            Errorcnt);
    fprintf(stderr, "For details, see log file(s) in the (fmt/base/mem) dir(s).\n");
  }
  return 0;
}
