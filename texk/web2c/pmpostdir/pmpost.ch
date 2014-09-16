@x
#include <kpathsea/kpathsea.h>
@y
#include <kpathsea/kpathsea.h>
#include <ptexenc/ptexenc.h>
char kanjioption[16];
@z

@x
@d TEX     "tex"
@y
@z

@x
#ifndef MPXCOMMAND
#define MPXCOMMAND "makempx"
#endif
@y
#ifdef UPMP
#define TEX "eptex"
#ifndef MPXCOMMAND
#define MPXCOMMAND "upmakempx"
#endif
#define mpost_name "upmpost"
#define dvitomp_name "updvitomp"
#else
#define TEX "euptex"
#ifndef MPXCOMMAND
#define MPXCOMMAND "pmakempx"
#endif
#define mpost_name "pmpost"
#define dvitomp_name "pdvitomp"
#endif
@z

@x
    if (cnf_cmd!=NULL && (strcmp (cnf_cmd, "1")!=0)) {
      if (mp_troff_mode(mp)!=0)
        cmd = concatn (cnf_cmd, " -troff ",
                     qmpname, " ", qmpxname, NULL);
      else if (mpost_tex_program!=NULL && *mpost_tex_program != '\0')
        cmd = concatn (cnf_cmd, " -tex=", mpost_tex_program, " ",
                     qmpname, " ", qmpxname, NULL);
@y
    if (cnf_cmd!=NULL && (strcmp (cnf_cmd, "1")!=0)) {
      if (kanjioption[0])
        cnf_cmd = concatn (cnf_cmd, " --kanji=", kanjioption, NULL);
      if (mp_troff_mode(mp))
        cmd = concatn (cnf_cmd, " -troff ",
                       qmpname, " ", qmpxname, NULL);
      else if (mpost_tex_program!=NULL && *mpost_tex_program != '\0')
        cmd = concatn (cnf_cmd, " -tex=", mpost_tex_program, " ",
                       qmpname, " ", qmpxname, NULL);
@z

@x
      if (mpost_tex_program != NULL && *mpost_tex_program != '\0') {
        maincmd = mpost_xstrdup(mpost_tex_program);
      } else {
        if (mpxmode == mpx_tex_mode) {
          s = kpse_var_value("TEX");
          if (s==NULL) s = kpse_var_value("MPXMAINCMD");
          if (s==NULL) s = mpost_xstrdup (TEX);
          maincmd = (char *)mpost_xmalloc (strlen(s)+strlen(default_args)+1);
          strcpy(maincmd,s);
@y
      if (mpost_tex_program != NULL && *mpost_tex_program != '\0') {
        if (kanjioption[0]) {
          maincmd = (char *)mpost_xmalloc (strlen(mpost_tex_program) +
                                   strlen(kanjioption) + 15);
          strcpy(maincmd, mpost_tex_program);
          strcat(maincmd, " --kanji=");
          strcat(maincmd, kanjioption);
        } else
          maincmd = mpost_xstrdup(mpost_tex_program);
      } else {
        if (mpxmode == mpx_tex_mode) {
          s = kpse_var_value("TEX");
          if (s==NULL) s = kpse_var_value("MPXMAINCMD");
          if (s==NULL) s = mpost_xstrdup (TEX);
          if (kanjioption[0])
            maincmd = (char *)mpost_xmalloc (strlen(s)+strlen(default_args)+
                                             strlen(kanjioption)+13);
          else
            maincmd = (char *)mpost_xmalloc (strlen(s)+strlen(default_args)+1);
          strcpy(maincmd,s);
          if (kanjioption[0]) {
            strcat(maincmd, " --kanji=");
            strcat(maincmd, kanjioption);
          }
@z

@x
        const char *banner = "% Written by metapost version ";
@y
        const char *banner = "% Written by " mpost_name " version ";
@z

@x
      const char *banner = "% Written by dvitomp version ";
@y
      const char *banner = "% Written by " dvitomp_name " version ";
@z

@x
      { "kpathsea-debug",            1, 0, 0 },
@y
      { "kpathsea-debug",            1, 0, 0 },
      { "kanji",                     1, 0, 0 },
@z

@x
        if (user_progname == NULL) 
	      user_progname = optarg;
      }
@y
        if (user_progname == NULL) 
	      user_progname = optarg;
      }
    } else if (ARGUMENT_IS ("kanji")) {
      strcpy(kanjioption, optarg);
      if(!set_enc_string(optarg, optarg)) {
        fprintf(stderr,"Ignoring unknown argument `%s' to --kanji", optarg);
      }
@z

@x
      { "no-kpathsea",               0, &nokpse, 1 },
@y
      { "no-kpathsea",               0, &nokpse, 1 },
      { "kanji",                     1, 0, 0 },
@z

@x
    } else if (option_is ("progname")) {
      user_progname = optarg;
@y
    } else if (option_is ("progname")) {
      user_progname = optarg;
    } else if (option_is ("kanji")) {
      strcpy (kanjioption, optarg);
      if(!set_enc_string(optarg, optarg)) {
        fprintf(stderr,"Ignoring unknown argument `%s' to --kanji", optarg);
      }
@z

@x
  fprintf(stdout, "This is dvitomp %s" WEB2CVERSION " (%s)\n", s, kpathsea_version_string);
else
  fprintf(stdout, "This is MetaPost %s" WEB2CVERSION " (%s)\n", s, kpathsea_version_string);
@y
  fprintf(stdout, "This is " dvitomp_name " %s" WEB2CVERSION " (%s)\n", s, kpathsea_version_string);
else
  fprintf(stdout, "This is " metapost_name " %s" WEB2CVERSION " (%s)\n", s, kpathsea_version_string);
@z

@x
"Usage: mpost [OPTION] [&MEMNAME] [MPNAME[.mp]] [COMMANDS]\n"
"       mpost --dvitomp DVINAME[.dvi] [MPXNAME[.mpx]]\n"
"\n"
"  Run MetaPost on MPNAME, usually creating MPNAME.NNN (and perhaps\n"
"  MPNAME.tfm), where NNN are the character numbers generated.\n"
"  Any remaining COMMANDS are processed as MetaPost input,\n"
"  after MPNAME is read.\n\n"
"  With a --dvitomp argument, MetaPost acts as DVI-to-MPX converter only.\n"
"  Call MetaPost with --dvitomp --help for option explanations.\n\n");
fprintf(stdout,
"  -ini                      be inimpost, for dumping mem files\n"
@y
"Usage: " mpost_name " [OPTION] [&MEMNAME] [MPNAME[.mp]] [COMMANDS]\n"
"       " mpost_name " --dvitomp DVINAME[.dvi] [MPXNAME[.mpx]]\n"
"\n"
"  Run " metapost_name " on MPNAME, usually creating MPNAME.NNN (and perhaps\n"
"  MPNAME.tfm), where NNN are the character numbers generated.\n"
"  Any remaining COMMANDS are processed as " metapost_name " input,\n"
"  after MPNAME is read.\n\n"
"  With a --dvitomp argument, " metapost_name " acts as DVI-to-MPX converter only.\n"
"  Call " metapost_name " with --dvitomp --help for option explanations.\n\n");
fprintf(stdout,
"  -ini                      be ini" mpost_name ", for dumping mem files\n"
@z

@x
"  -jobname=STRING           set the job name to STRING\n"
@y
"  -jobname=STRING           set the job name to STRING\n"
"  -kanji=STRING             set the Japanese encoding to STRING\n"
@z

@x
"  -version                  output version information and exit\n"
"\n"
"Email bug reports to mp-implementors@@tug.org.\n"
@y
"  -version                  output version information and exit\n"
@z

@x
  fprintf(stdout, "This is dvitomp %s" WEB2CVERSION " (%s)\n", s, kpathsea_version_string);
else
  fprintf(stdout, "This is MetaPost %s" WEB2CVERSION " (%s)\n", s, kpathsea_version_string);
@y
  fprintf(stdout, "This is " dvitomp_name " %s" WEB2CVERSION " (%s)\n", s, kpathsea_version_string);
else
  fprintf(stdout, "This is " metapost_name " %s" WEB2CVERSION " (%s)\n", s, kpathsea_version_string);
@z

@x
"Usage: dvitomp DVINAME[.dvi] [MPXNAME[.mpx]]\n"
"       mpost --dvitomp DVINAME[.dvi] [MPXNAME[.mpx]]\n"
@y
"Usage: " dvitomp_name " DVINAME[.dvi] [MPXNAME[.mpx]]\n"
"       " mpost_name " --dvitomp DVINAME[.dvi] [MPXNAME[.mpx]]\n"
@z

@x
"  -progname=STRING          set program name to STRING\n"
@y
"  -progname=STRING          set program name to STRING\n"
#ifdef UPMP
"  -kanji=STRING             set kanji encoding (STRING=euc|jis|sjis|utf8|uptex)\n"
#else
"  -kanji=STRING             set kanji encoding (sjis, jis, euc, utf8)\n"
#endif
@z

@x
  fprintf(stdout, "dvitomp (MetaPost) %s" WEB2CVERSION " (%s)\n", s, kpathsea_version_string);
else
  fprintf(stdout, "MetaPost %s" WEB2CVERSION " (%s)\n", s, kpathsea_version_string);
@y
  fprintf(stdout, dvitomp_name " (" metapost_name ") %s" WEB2CVERSION " (%s)\n", s, kpathsea_version_string);
else
  fprintf(stdout, metapost_name " %s" WEB2CVERSION " (%s)\n", s, kpathsea_version_string);
@z

@x
"Current maintainer of MetaPost: Luigi Scarso.\n\n"
@y
"Current maintainer of MetaPost: Luigi Scarso.\n"
#ifdef UPMP
"Authors of upMetaPost: Michio Matsuyama, Hideyuki Suzuki, Takuji Tanaka.\n\n"
#else
"Authors of pMetaPost: Michio Matsuyama, Hideyuki Suzuki.\n\n"
#endif
@z

@x
  const char * banner = "This is MetaPost, version ";
@y
  const char * banner = "This is " metapost_name ", version ";
@z

@x
  if (dvitomp_only) {
    @<Read and set dvitomp command line options@>;
  } else {
@y
  kanjioption[0] = '\0';
#ifdef UPMP
  enable_UPTEX (true);
  set_enc_string("uptex", "uptex");
#else
  enable_UPTEX (false);
#if defined(WIN32)
  set_enc_string("sjis", "sjis");
#else
  set_enc_string("utf8", "euc");
#endif
#endif

  if (dvitomp_only) {
    @<Read and set dvitomp command line options@>;
  } else {
#ifdef WIN32
    options->mem_name =
      strlwr(kpse_program_basename(argv[0]));
#else
    options->mem_name =
      kpse_program_basename(argv[0]);
#endif
@z

@x
  if(putenv(xstrdup("engine=metapost")))
@y
  if(putenv(xstrdup("engine=" mpost_name)))
@z
