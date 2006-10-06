/****************************************************************************\
 Part of the XeTeX typesetting system
 copyright (c) 1994-2005 by SIL International
 written by Jonathan Kew

 This software is distributed under the terms of the Common Public License,
 version 1.0.
 For details, see <http://www.opensource.org/licenses/cpl1.0.php> or the file
 cpl1.0.txt included with the software.
\****************************************************************************/

/*
	xdv2pdf_main
	
	Main file for xdv2pdf
	Written in plain C for easier linkage to kpathsea library
*/

#define MAC_OS_X_VERSION_MIN_REQUIRED	1020

#include <kpathsea/config.h>

#include <kpathsea/c-fopen.h>
#include <kpathsea/c-pathch.h>
#include <kpathsea/c-vararg.h>
#include <kpathsea/cnf.h>
#include <kpathsea/concatn.h>
#include <kpathsea/default.h>
#include <kpathsea/expand.h>
#include <kpathsea/fontmap.h>
#include <kpathsea/paths.h>
#include <kpathsea/pathsearch.h>
#include <kpathsea/tex-file.h>
#include <kpathsea/tex-make.h>
#include <kpathsea/variable.h>

#include "xdv_kpse_formats.h"

/* The program name, for `.PROG' construct in texmf.cnf.  (-program) */
string progname = NULL;

extern int	xdv2pdf(int argc, char** argv);

#ifndef DEFAULT_OTFFONTS
#define DEFAULT_OTFFONTS ".:/usr/local/teTeX/share/texmf/fonts/otf//:/usr/TeX/texmf/fonts/otf//"
#endif

#define INIT_FORMAT(text, default_path, envs) \
  FMT_INFO.type = text; \
  init_path (&FMT_INFO, default_path, envs, NULL)

#define OTFFONTS_ENVS "OTFFONTS"

#define TFM_ENVS "TFMFONTS", "TEXFONTS"
#define CNF_ENVS "TEXMFCNF"
#define DB_ENVS "TEXMFDBS"
#define DVIPS_CONFIG_ENVS "TEXCONFIG"
#define FONTMAP_ENVS "TEXFONTMAPS", "TEXFONTS", DVIPS_CONFIG_ENVS /* try TEXCONFIG for teTeX 2 */
#define TEX_PS_HEADER_ENVS "TEXPSHEADERS", "PSHEADERS"
#define ENC_ENVS "ENCFONTS", "TEXFONTS", TEX_PS_HEADER_ENVS
#define TYPE1_ENVS "T1FONTS", "T1INPUTS", "TEXFONTS", TEX_PS_HEADER_ENVS

#define FMT_INFO kpse_format_info[format]
/* Call kpse_set_add_suffixes.  */
#define SUFFIXES(args) kpse_set_suffixes(format, false, args, NULL)
#define ALT_SUFFIXES(args) kpse_set_suffixes(format, true, args, NULL)

/* Macro subroutines for `init_path'.  TRY_ENV checks if an envvar ENAME
   is set and non-null, and sets var to ENAME if so.  */
#define TRY_ENV(ename) do { \
  string evar = ename; \
} while (0)

/* And EXPAND_DEFAULT calls kpse_expand_default on try_path and the
   present info->path.  */
#define EXPAND_DEFAULT(try_path, source_string)			\
  if (try_path) {						\
      info->raw_path = try_path;				\
      info->path = kpse_expand_default (try_path, info->path);	\
      info->path_source = source_string;			\
  }

static string
remove_dbonly P1C(const_string, path)
{
  string ret = XTALLOC(strlen (path) + 1, char), q=ret;
  const_string p=path;
  boolean new_elt=true;

  while (*p) {
    if (new_elt && *p && *p == '!' && *(p+1) == '!')
      p += 2;
    else {
      new_elt = (*p == ENV_SEP);
      *q++ = *p++;
    }
  }
  *q = '\0';
  return(ret);
}

/* Find the final search path to use for the format entry INFO, given
   the compile-time default (DEFAULT_PATH), and the environment
   variables to check (the remaining arguments, terminated with NULL).
   We set the `path' and `path_source' members of INFO.  The
   `client_path' member must already be set upon entry.  */

static void
init_path PVAR2C(kpse_format_info_type *, info, const_string, default_path, ap)
{
  string env_name;
  string var = NULL;
  
  info->default_path = default_path;

  /* First envvar that's set to a nonempty value will exit the loop.  If
     none are set, we want the first cnf entry that matches.  Find the
     cnf entries simultaneously, to avoid having to go through envvar
     list twice -- because of the PVAR?C macro, that would mean having
     to create a str_list and then use it twice.  Yuck.  */
  while ((env_name = va_arg (ap, string)) != NULL) {
    /* Since sh doesn't like envvar names with `.', check PATH_prog
       as well as PATH.prog.  */
    if (!var) {
      /* Try PATH.prog. */
      string evar = concat3 (env_name, ".", kpse_program_name);
      string env_value = getenv (evar);
      if (env_value && *env_value) {
        var = evar;
      } else {
        free (evar);
        /* Try PATH_prog. */
        evar = concat3 (env_name, "_", kpse_program_name);
        env_value = getenv (evar);
        if (env_value && *env_value) {
          var = evar;
        } else {
          free (evar);
          /* Try simply PATH.  */
          env_value = getenv (env_name);
          if (env_value && *env_value) {
            var = env_name;        
          }
        }
      }
    }
    
    /* If we are initializing the cnf path, don't try to get any
       values from the cnf files; that's infinite loop time.  */
    if (!info->cnf_path && info != &kpse_format_info[kpse_cnf_format])
      info->cnf_path = kpse_cnf_get (env_name);
      
    if (var && info->cnf_path)
      break;
  }
  va_end (ap);
  
  /* Expand any extra :'s.  For each level, we replace an extra : with
     the path at the next lower level.  For example, an extra : in a
     user-set envvar should be replaced with the path from the cnf file.
     things are complicated because none of the levels above the very
     bottom are guaranteed to exist.  */

  /* Assume we can reliably start with the compile-time default.  */
  info->path = info->raw_path = info->default_path;
  info->path_source = "compile-time paths.h";

  EXPAND_DEFAULT (info->cnf_path, "texmf.cnf");
  EXPAND_DEFAULT (info->client_path, "program config file");
  if (var)
    EXPAND_DEFAULT (getenv (var), concat (var, " environment variable"));
  EXPAND_DEFAULT (info->override_path, "application override variable");
  info->path = kpse_brace_expand (info->path);
}
}	/* extra closing brace to match with PVAR2C in function header */

int
main(int argc, char** argv)
{
	kpse_file_format_type	format;

	kpse_set_program_name (argv[0], progname);
    
    /* It seems to be critical that I do a kpse_cnf_get to trigger the reading of texmf.cnf files
        BEFORE the INIT_FORMAT stuff below. I don't really understand all the interactions
        of search paths, config files, etc., but for now this is working for me. */
    kpse_cnf_get("TETEXDIR");
    
    format = kpse_cnf_format;
    INIT_FORMAT ("cnf", DEFAULT_TEXMFCNF, CNF_ENVS);
    SUFFIXES (".cnf");

    format = kpse_db_format;
    INIT_FORMAT ("ls-R", DEFAULT_TEXMFDBS, DB_ENVS);
    SUFFIXES ("ls-R");
    FMT_INFO.path = remove_dbonly (FMT_INFO.path);
    
    format = xdv_kpse_pfb_format;
	INIT_FORMAT ("type1 fonts", DEFAULT_T1FONTS, TYPE1_ENVS);
	FMT_INFO.binmode = true;
	FMT_INFO.suffix_search_only = true;
	SUFFIXES (".pfb");

    format = xdv_kpse_otf_format;
	INIT_FORMAT ("otf fonts", DEFAULT_OTFFONTS, OTFFONTS_ENVS);
	FMT_INFO.binmode = true;
	FMT_INFO.suffix_search_only = true;
	SUFFIXES (".otf");

    format = xdv_kpse_tfm_format;
	INIT_FORMAT ("tfm", DEFAULT_TFMFONTS, TFM_ENVS);
	FMT_INFO.binmode = true;
	FMT_INFO.suffix_search_only = true;
	SUFFIXES (".tfm");

    format = xdv_kpse_font_map_format;
	INIT_FORMAT ("map", DEFAULT_TEXFONTMAPS, FONTMAP_ENVS);
	FMT_INFO.suffix_search_only = true;
	SUFFIXES (".map");

    format = xdv_kpse_enc_format;
	INIT_FORMAT ("enc files", DEFAULT_ENCFONTS, ENC_ENVS);
	FMT_INFO.suffix_search_only = true;
	SUFFIXES (".enc");

	return xdv2pdf(argc, argv);
}
