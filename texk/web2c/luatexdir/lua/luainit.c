#include <sys/stat.h>

#include "luatex-api.h"
#include <ptexlib.h>

#include <luatexdir/luatexextra.h>

#ifndef program_invocation_name
/*  char *program_invocation_name = NULL; */
#endif

extern void parse_src_specials_option (char *n) ;

const_string LUATEX_IHELP[] = {
    "Usage: luatex --lua=FILE [OPTION]... [TEXNAME[.tex]] [COMMANDS]",
    "   or: luatex --lua=FILE [OPTION]... \\FIRST-LINE",
    "   or: luatex --lua=FILE [OPTION]... &FMT ARGS",
    "  Run luaTeX on TEXNAME, usually creating TEXNAME.pdf.",
    "  Any remaining COMMANDS are processed as luatex input, after TEXNAME is read.",
    "",
    "  Alternatively, if the first non-option argument begins with a backslash,",
    "  interpret all non-option arguments as a line of luatex input.",
    "",
    "  Alternatively, if the first non-option argument begins with a &, the",
    "  next word is taken as the FMT to read, overriding all else.  Any",
    "  remaining arguments are processed as above.",
    "",
    "  If no arguments or options are specified, prompt for input.",
    "",
    "  --lua=FILE               the lua initialization file",
    "  --safer                  disable some easily exploitable lua commands",
    "  --fmt=FORMAT             load the format file FORMAT",
    "  --ini                    be initex, for dumping formats",
    "  --help                   display this help and exit",
    "  --version                output version information and exit",
    "",
    "  Alternate behaviour models can be obtained by special switches",
    "",
    "  --luaonly                run a lua file, then exit",
    "  --luaconly               byte-compile a lua file, then exit",
    NULL
};

static void
prepare_cmdline(lua_State * L, char **argv, int argc, int zero_offset)
{
    int i;
    luaL_checkstack(L, argc + 3, "too many arguments to script");
    lua_createtable(L, 0, 0);
    for (i = 0; i < argc; i++) {
	  lua_pushstring(L, argv[i]);
	  lua_rawseti(L, -2, (i-zero_offset));
    }
    lua_setglobal(L, "arg");
    return;
}

extern string dump_name;
extern const_string c_job_name;
extern boolean srcspecialsoption;
extern char *last_source_name;
extern int last_lineno;

string input_name = NULL;

static string user_progname = NULL;

extern char *ptexbanner;
extern int program_name_set; /* in lkpselib.c */

/* for topenin() */
extern char **argv;
extern int argc;

char *startup_filename = NULL;
int lua_only = 0;
int lua_offset = 0;

int safer_option = 0;

/* Reading the options.  */

/* Test whether getopt found an option ``A''.
   Assumes the option index is in the variable `option_index', and the
   option table in a variable `long_options'.  */
#define ARGUMENT_IS(a) STREQ (long_options[option_index].name, a)

/* SunOS cc can't initialize automatic structs, so make this static.  */
static struct option long_options[]
 = {  { "fmt",                       1, 0, 0},
      { "lua",                       1, 0, 0},
      { "luaonly",                   0, 0, 0},
      { "safer",                     0, &safer_option, 1},
      { "help",                      0, 0, 0 },
      { "ini",                       0, &ini_version, 1 },
      { "interaction",               1, 0, 0 },
      { "halt-on-error",             0, &haltonerrorp, 1 },
      { "kpathsea-debug",            1, 0, 0 },
      { "progname",                  1, 0, 0 },
      { "version",                   0, 0, 0 },
      { "recorder",                  0, &recorder_enabled, 1 },
      { "etex",                      0, 0, 0 },
      { "output-comment",            1, 0, 0 },
      { "output-directory",          1, 0, 0 },
      { "draftmode",                 0, 0, 0 },
      { "output-format",             1, 0, 0 },
      { "shell-escape",              0, &shellenabledp, 1 },
      { "no-shell-escape",           0, &shellenabledp, -1 },
      { "debug-format",              0, &debug_format_file, 1 },
      { "src-specials",              2, 0, 0 },
      { "file-line-error-style",     0, &filelineerrorstylep, 1 },
      { "no-file-line-error-style",  0, &filelineerrorstylep, -1 },
      /* Shorter option names for the above. */
      { "file-line-error",           0, &filelineerrorstylep, 1 },
      { "no-file-line-error",        0, &filelineerrorstylep, -1 },
      { "jobname",                   1, 0, 0 },
      { "parse-first-line",          0, &parsefirstlinep, 1 },
      { "no-parse-first-line",       0, &parsefirstlinep, -1 },
      { "translate-file",            1, 0, 0 },
      { "default-translate-file",    1, 0, 0 },
      { "8bit",                      0, 0, 0 },
      { "mktex",                     1, 0, 0 },
      { "no-mktex",                  1, 0, 0 },
{0, 0, 0, 0}
};

static void
parse_options(int argc, char **argv)
{
  int g;			/* `getopt' return code.  */
  int option_index;
  char *firstfile = NULL;
  opterr = 0;			/* dont whine */
  for (;;) {
	g = getopt_long_only(argc, argv, "+", long_options, &option_index);
	
	if (g == -1)		/* End of arguments, exit the loop.  */
	    break;
	if (g == '?')		/* Unknown option.  */
	  continue;
	
	assert(g == 0);		/* We have no short option names.  */
	
	if (ARGUMENT_IS("luaonly")) {
	  lua_only = 1;
	  lua_offset = optind;
	  luainit = 1 ;
	} else if (ARGUMENT_IS("lua")) {
	  startup_filename = optarg;
	  lua_offset = (optind-1);
	  luainit = 1 ;
	  
	} else if (ARGUMENT_IS ("kpathsea-debug")) {
      kpathsea_debug |= atoi (optarg);

	} else if (ARGUMENT_IS("progname")) {
	  user_progname = optarg;
	  
    } else if (ARGUMENT_IS ("jobname")) {
      c_job_name = optarg;

	} else if (ARGUMENT_IS("fmt")) {
	  dump_name = optarg;
	  
    } else if (ARGUMENT_IS ("output-directory")) {
      output_directory = optarg;

    } else if (ARGUMENT_IS ("output-comment")) {
      unsigned len = strlen (optarg);
      if (len < 256) {
        output_comment = optarg;
      } else {
        WARNING2 ("Comment truncated to 255 characters from %d. (%s)",
                  len, optarg);
        output_comment = (string)xmalloc (256);
        strncpy (output_comment, optarg, 255);
        output_comment[255] = 0;
      }

    } else if (ARGUMENT_IS ("src-specials")) {
       last_source_name = xstrdup("");
       /* Option `--src" without any value means `auto' mode. */
       if (optarg == NULL) {
         insertsrcspecialeverypar = true;
         insertsrcspecialauto = true;
         srcspecialsoption = true;
         srcspecialsp = true;
       } else {
          parse_src_specials_option(optarg);
       }

    } else if (ARGUMENT_IS ("output-format")) {
       pdf_output_option = 1;
       if (strcmp(optarg, "dvi") == 0) {
         pdf_output_value = 0;
       } else if (strcmp(optarg, "pdf") == 0) {
         pdf_output_value = 2;
       } else {
         WARNING1 ("Ignoring unknown value `%s' for --output-format", optarg);
         pdf_output_option = 0;
       }

    } else if (ARGUMENT_IS ("draftmode")) {
      pdf_draftmode_option = 1;
      pdf_draftmode_value = 1;

    } else if (ARGUMENT_IS ("mktex")) {
      kpse_maketex_option (optarg, true);

    } else if (ARGUMENT_IS ("no-mktex")) {
      kpse_maketex_option (optarg, false);

    } else if (ARGUMENT_IS ("interaction")) {
        /* These numbers match @d's in *.ch */
      if (STREQ (optarg, "batchmode")) {
        interactionoption = 0;
      } else if (STREQ (optarg, "nonstopmode")) {
        interactionoption = 1;
      } else if (STREQ (optarg, "scrollmode")) {
        interactionoption = 2;
      } else if (STREQ (optarg, "errorstopmode")) {
        interactionoption = 3;
      } else {
        WARNING1 ("Ignoring unknown argument `%s' to --interaction", optarg);
      }
      
	} else if (ARGUMENT_IS("help")) {
	  usagehelp(LUATEX_IHELP, BUG_ADDRESS);
	  
	} else if (ARGUMENT_IS("version")) {
	  char *versions;
	  initversionstring(&versions); 
          print_version_banner();

	  puts(
"\n\nLuaTeX merges and builds upon (parts of) the code from these projects:\n\n"
"tex       by Donald Knuth\n"
"etex      by Peter Breitenlohner, Phil Taylor and friends\n"
"omega     by John Plaice and Yannis Haralambous\n"
"aleph     by Giuseppe Bilotta\n"
"pdftex    by Han The Thanh and friends\n"
"kpathsea  by Karl Berry, Olaf Weber and others\n"
"lua       by Roberto Ierusalimschy, Waldemar Celes,\n"
"             Luiz Henrique de Figueiredo\n"
"metapost  by John Hobby, Taco Hoekwater and friends.\n"
"xpdf      by Derek Noonberg (partial)\n"
"fontforge by George Williams (partial)\n\n"
"Some extensions to lua and additional lua libraries are used, as well as\n"
"libraries for graphic inclusion. More details can be found in the source.\n"
"Code development was sponsored by a grant from Colorado State University\n"
"via the 'oriental tex' project, the TeX User Groups, and donations.\n\n"
"The LuaTeX team is Hans Hagen, Hartmut Henkel, Taco Hoekwater.\n\n"
"There is NO warranty. Redistribution of this software is covered by\n"
"the terms of the GNU General Public License, version 2. For more\n"
"information about these matters, see the file named COPYING and\n"
"the LuaTeX source.\n\n"
"Copyright 2008 Taco Hoekwater, the LuaTeX Team.\n");

	  puts(versions);
          uexit(0);	  
	}
  }
  /* attempt to find dump_name */
  if (argv[optind] && argv[optind][0] == '&') {
	dump_name = strdup(argv[optind] + 1);
  } else if (argv[optind] && argv[optind][0] != '\\') {
	if (argv[optind][0] == '*') {
	  input_name = strdup(argv[optind] + 1);
	} else {
	  firstfile = strdup(argv[optind]);
	  if (lua_only) {
		startup_filename = firstfile;
	  }  else {
		if ((strstr(firstfile,".lua") == firstfile+strlen(firstfile)-4) ||
			(strstr(firstfile,".luc") == firstfile+strlen(firstfile)-4) ||
            (strstr(firstfile,".LUA") == firstfile+strlen(firstfile)-4) ||
			(strstr(firstfile,".LUC") == firstfile+strlen(firstfile)-4) ||
			(strstr(argv[0],"luatexlua") != NULL) ||
            (strstr(argv[0],"texlua") != NULL)) {
		  startup_filename = firstfile;
		  lua_only = 1;
		  lua_offset = optind;		  
		  luainit = 1 ;
		} else {
		  input_name = firstfile;
		}
	  }
	}
  } else {
	if ((strstr(argv[0],"luatexlua") != NULL)||
        (strstr(argv[0],"texlua") != NULL)) {
	  lua_only = 1;
	  luainit = 1 ;
	}
  }
}

/* test for readability */
#define is_readable(a) (stat(a,&finfo)==0) && S_ISREG(finfo.st_mode) &&  \
  (f=fopen(a,"r")) != NULL && !fclose(f)

char *
find_filename(char *name, char *envkey)
{
    struct stat finfo;
    char *dirname = NULL;
    char *filename = NULL;
    FILE *f;
    if (is_readable(name)) {
	return name;
    } else {
	dirname = getenv(envkey);
	if ((dirname != NULL) && strlen(dirname)) {
	    dirname = strdup(getenv(envkey));
	    if (*(dirname + strlen(dirname) - 1) == '/') {
		*(dirname + strlen(dirname) - 1) = 0;
	    }
	    filename = xmalloc(strlen(dirname) + strlen(name) + 2);
	    filename = concat3(dirname, "/", name);
	    if (is_readable(filename)) {
		xfree(dirname);
		return filename;
	    }
	    xfree(filename);
	}
    }
    return NULL;
}


void 
init_kpse (void) {

  if (!user_progname) {
	user_progname = dump_name;
  } else if (!dump_name) {
	dump_name = user_progname;
  }
  if (!user_progname) {
	if (ini_version) {
	  user_progname = input_name;
	} else {
	  if(!startup_filename) {
		if (!dump_name) {
		  dump_name = strdup(argv[0]);
		  user_progname = dump_name;
		}
	  }
	}
  }
  if (!user_progname) {
	  fprintf(stdout,
			  "kpathsea mode needs a --progname or --fmt switch\n");
	  exit(1);
  } 
  kpse_set_program_name(argv[0], user_progname);
  program_name_set=1;
}

void
fix_dumpname (void) {
  int dist;
  if (dump_name) {
	/* adjust array for Pascal and provide extension, if needed */
	dist = strlen(dump_name) - strlen(DUMP_EXT);
	if (strstr(dump_name, DUMP_EXT) == dump_name + dist)
	  DUMP_VAR = concat(" ", dump_name);
	else
	  DUMP_VAR = concat3(" ", dump_name, DUMP_EXT);
	DUMP_LENGTH_VAR = strlen(DUMP_VAR + 1);
  } else {
	/* For dump_name to be NULL is a bug.  */
	if (!ini_version)
	  abort();
  }
}

void
lua_initialize(int ac, char **av)
{

  char *given_file = NULL;
  int kpse_init;
  int tex_table_id;
  int pdf_table_id;
  int token_table_id;
  int node_table_id;
  /* Save to pass along to topenin.  */
  argc = ac;
  argv = av;
	
  ptexbanner = BANNER;
  program_invocation_name = argv[0];

  /* be 'luac' */
  if (argc>1 &&
      (STREQ(argv[0],"texluac") ||
       STREQ(argv[1],"--luaconly") ||
       STREQ(argv[1],"--luac"))) {
    exit(luac_main(ac,av));
  }

  /* Must be initialized before options are parsed.  */
  interactionoption = 4;
  dump_name = NULL;
  /* parse commandline */
  parse_options(ac, av);

  /* make sure that the locale is 'sane' (for lua) */
  putenv("LC_CTYPE=C");
  putenv("LC_COLLATE=C");
  putenv("LC_NUMERIC=C");

  /* this is sometimes needed */
  putenv("engine=luatex");

  luainterpreter(0);
	  
  prepare_cmdline(Luas[0], argv, argc, lua_offset);	/* collect arguments */

  if (startup_filename != NULL) {
    given_file = xstrdup(startup_filename);
    startup_filename = find_filename(startup_filename, "LUATEXDIR");
  }
  /* now run the file */
  if (startup_filename != NULL) {
    /* hide the 'tex' and 'pdf' table */
    tex_table_id = hide_lua_table(Luas[0], "tex");
    token_table_id = hide_lua_table(Luas[0], "token");
    node_table_id = hide_lua_table(Luas[0], "node");
    pdf_table_id = hide_lua_table(Luas[0], "pdf");

    if (luaL_loadfile(Luas[0], startup_filename)) {
      fprintf(stdout, "%s\n",lua_tostring(Luas[0], -1));
      exit(1);
    }
    if (lua_pcall(Luas[0], 0, 0, 0)) {
      fprintf(stdout, "%s\n",lua_tostring(Luas[0], -1));
      exit(1);
    }
    /* no filename? quit now! */
    if (!input_name) {
      get_lua_string("texconfig", "jobname", &input_name);
    }
    if (!dump_name) {
      get_lua_string("texconfig", "formatname", &dump_name);
    }
    if ((lua_only) || ((!input_name) && (!dump_name))) {
      exit(0);
    }
    /* unhide the 'tex' and 'pdf' table */
    unhide_lua_table(Luas[0], "tex", tex_table_id);
    unhide_lua_table(Luas[0], "pdf", pdf_table_id);
    unhide_lua_table(Luas[0], "token", token_table_id);
    unhide_lua_table(Luas[0], "node", node_table_id);
	  
    /* kpse_init */
    kpse_init = -1;
    get_lua_boolean("texconfig", "kpse_init", &kpse_init);

    if (kpse_init != 0) {
      init_kpse();
    }
    /* prohibit_file_trace (boolean) */
    tracefilenames = 1;
    get_lua_boolean("texconfig", "trace_file_names", &tracefilenames);
	  
    /* src_special_xx */
    insertsrcspecialauto = insertsrcspecialeverypar =
      insertsrcspecialeveryparend = insertsrcspecialeverycr =
      insertsrcspecialeverymath = insertsrcspecialeveryhbox =
      insertsrcspecialeveryvbox = insertsrcspecialeverydisplay =
      false;
    get_lua_boolean("texconfig", "src_special_auto",
		    &insertsrcspecialauto);
    get_lua_boolean("texconfig", "src_special_everypar",
		    &insertsrcspecialeverypar);
    get_lua_boolean("texconfig", "src_special_everyparend",
		    &insertsrcspecialeveryparend);
    get_lua_boolean("texconfig", "src_special_everycr",
		    &insertsrcspecialeverycr);
    get_lua_boolean("texconfig", "src_special_everymath",
		    &insertsrcspecialeverymath);
    get_lua_boolean("texconfig", "src_special_everyhbox",
		    &insertsrcspecialeveryhbox);
    get_lua_boolean("texconfig", "src_special_everyvbox",
		    &insertsrcspecialeveryvbox);
    get_lua_boolean("texconfig", "src_special_everydisplay",
		    &insertsrcspecialeverydisplay);

    srcspecialsp = insertsrcspecialauto | insertsrcspecialeverypar |
      insertsrcspecialeveryparend | insertsrcspecialeverycr |
      insertsrcspecialeverymath | insertsrcspecialeveryhbox |
      insertsrcspecialeveryvbox | insertsrcspecialeverydisplay;

    /* file_line_error */
    filelineerrorstylep = false;
    get_lua_boolean("texconfig", "file_line_error",
		    &filelineerrorstylep);

    /* halt_on_error */
    haltonerrorp = false;
    get_lua_boolean("texconfig", "halt_on_error", &haltonerrorp);
	  
    fix_dumpname();
  } else {
    if (luainit) {
      if (given_file) {
	fprintf(stdout, "%s file %s not found\n", (lua_only ? "Script" : "Configuration"), given_file);
      } else {
	fprintf(stdout, "No %s file given\n", (lua_only ? "script" : "configuration"));
      }
      exit(1);
    } else {
      /* init */
      init_kpse();
      fix_dumpname();
    }
  }
}
