/* types.h: general types for kpathsea.

   Copyright 1993, 1995, 1996, 2005, 2008-2014, 2016 Karl Berry.

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

#ifndef KPATHSEA_TYPES_H
#define KPATHSEA_TYPES_H

/* Our string, boolean, etc.  */
#include <kpathsea/simpletypes.h>

/* Required until all programs use the new API, if ever.  */
#define KPSE_COMPAT_API 1

#include <stdio.h> /* for FILE* */

/* Declare int64_t and uint64_t, and define PRId64 etc.  */
#ifdef HAVE_INTTYPES_H
# include <inttypes.h>
#endif
#ifdef HAVE_STDINT_H
# include <stdint.h>
#endif
#if (!defined __cplusplus || defined __STDC_FORMAT_MACROS) && !defined PRId64
# if SIZEOF_LONG == 8
#  define __PRI64_PREFIX	"l"
# else
#  define __PRI64_PREFIX	"ll"
# endif
# define PRId64		__PRI64_PREFIX "d"
# define PRIi64		__PRI64_PREFIX "i"
# define PRIo64		__PRI64_PREFIX "o"
# define PRIu64		__PRI64_PREFIX "u"
# define PRIx64		__PRI64_PREFIX "x"
# define PRIX64		__PRI64_PREFIX "X"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* function pointer prototype definitions for recorder */
typedef void (*p_record_input) (const_string);
typedef void (*p_record_output) (const_string);

#ifdef __cplusplus
}
#endif

/* the cache structure from elt-dirs.c */
#include <kpathsea/str-llist.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
  const_string key;
  str_llist_type *value;
} cache_entry;

/* from variable.c  */
typedef struct {
  const_string var;
  boolean expanding;
} expansion_type;

#ifdef __cplusplus
}
#endif


#include <kpathsea/hash.h>
#include <kpathsea/str-list.h>

#ifdef __cplusplus
extern "C" {
#endif

/* from old tex-file.h */

/* We put the glyphs first so we don't waste space in an array in
   tex-glyph.c.  Accompany a new format here with appropriate changes in
   tex-file.c and kpsewhich.c (the suffix variable).  */
typedef enum
{
  kpse_gf_format,
  kpse_pk_format,
  kpse_any_glyph_format,        /* ``any'' meaning gf or pk */
  kpse_tfm_format,
  kpse_afm_format,
  kpse_base_format,
  kpse_bib_format,
  kpse_bst_format,
  kpse_cnf_format,
  kpse_db_format,
  kpse_fmt_format,
  kpse_fontmap_format,
  kpse_mem_format,
  kpse_mf_format,
  kpse_mfpool_format,
  kpse_mft_format,
  kpse_mp_format,
  kpse_mppool_format,
  kpse_mpsupport_format,
  kpse_ocp_format,
  kpse_ofm_format,
  kpse_opl_format,
  kpse_otp_format,
  kpse_ovf_format,
  kpse_ovp_format,
  kpse_pict_format,
  kpse_tex_format,
  kpse_texdoc_format,
  kpse_texpool_format,
  kpse_texsource_format,
  kpse_tex_ps_header_format,
  kpse_troff_font_format,
  kpse_type1_format,
  kpse_vf_format,
  kpse_dvips_config_format,
  kpse_ist_format,
  kpse_truetype_format,
  kpse_type42_format,
  kpse_web2c_format,
  kpse_program_text_format,
  kpse_program_binary_format,
  kpse_miscfonts_format,
  kpse_web_format,
  kpse_cweb_format,
  kpse_enc_format,
  kpse_cmap_format,
  kpse_sfd_format,
  kpse_opentype_format,
  kpse_pdftex_config_format,
  kpse_lig_format,
  kpse_texmfscripts_format,
  kpse_lua_format,
  kpse_fea_format,
  kpse_cid_format,
  kpse_mlbib_format,
  kpse_mlbst_format,
  kpse_clua_format,
  kpse_ris_format,
  kpse_bltxml_format,
  kpse_last_format /* one past last index */
} kpse_file_format_type;


/* Perhaps we could use this for path values themselves; for now, we use
   it only for the program_enabled_p value.  */
typedef enum
{
  kpse_src_implicit,   /* C initialization to zero */
  kpse_src_compile,    /* configure/compile-time default */
  kpse_src_texmf_cnf,  /* texmf.cnf, the kpathsea config file */
  kpse_src_client_cnf, /* application config file, e.g., config.ps */
  kpse_src_env,        /* environment variable */
  kpse_src_x,          /* X Window System resource */
  kpse_src_cmdline     /* command-line option */
} kpse_src_type;


/* For each file format, we record the following information.  The main
   thing that is not part of this structure is the environment variable
   lists. They are used directly in tex-file.c. We could incorporate
   them here, but it would complicate the code a bit. We could also do
   it via variable expansion, but not now, maybe not ever:
   ${PKFONTS-${TEXFONTS-/usr/local/lib/texmf/fonts//}}.  */

typedef struct
{
  const_string type;            /* Human-readable description.  */
  string path;                  /* The search path to use.  */
  const_string raw_path;        /* Pre-$~ (but post-default) expansion.  */
  const_string path_source;     /* Where the path started from.  */
  const_string override_path;   /* From client environment variable.  */
  const_string client_path;     /* E.g., from dvips's config.ps.  */
  const_string cnf_path;        /* From texmf.cnf.  */
  const_string default_path;    /* If all else fails.  */
  const_string *suffix;         /* For kpse_find_file to check for/append.  */
  const_string *alt_suffix;     /* More suffixes to check for.  */
  boolean suffix_search_only;   /* Only search with a suffix?  */
  const_string program;         /* ``mktexpk'', etc.  */
  int argc;                     /* Count of standard arguments.  */
  const_string *argv;           /* Standard arguments to `program'.  */
  boolean program_enabled_p;    /* Invoke `program'?  */
  kpse_src_type program_enable_level; /* Who said to invoke `program'.  */
  boolean binmode;              /* Open files in binary mode?  */
} kpse_format_info_type;

#if defined(WIN32) && !defined(__MINGW32__)
struct passwd {
  char *pw_name;
  char *pw_passwd;
  int   pw_uid;
  int   pw_gid;
  int   pw_quota;
  char *pw_gecos;
  char *pw_dir;
  char *pw_shell;
};
#endif /* WIN32 && !__MINGW32 */

typedef struct kpathsea_instance *kpathsea;

typedef struct kpathsea_instance {
    /* from cnf.c */
    p_record_input record_input;        /* for --recorder */
    p_record_output record_output;      /* for --recorder */
    hash_table_type cnf_hash;           /* used by read_all_cnf */
    boolean doing_cnf_init;             /* for kpse_cnf_get */
    /* from db.c */
    hash_table_type db;                 /* The hash table for all ls-R's */
    hash_table_type alias_db;           /* The hash table for the aliases */
    str_list_type db_dir_list;          /* list of ls-R's */
    /* from debug.c */
    unsigned debug;                     /* for --kpathsea-debug */
    /* from dir.c */
    hash_table_type link_table;         /* a hash of links-per-dir */
    /* from elt-dir.c */
    cache_entry *the_cache;
    unsigned cache_length;
    /* from fontmap.c */
    hash_table_type map;                /* the font mapping hash */
    const_string map_path;              /* path for kpse_fontmap_format */
    /* from hash.c */
    /* Print the hash values as integers if this is nonzero.  */
    boolean debug_hash_lookup_int;
    /* from path-elt.c */
    string elt;                         /* static buffer for return value */
    unsigned elt_alloc;
    const_string path;                  /* path we're currently working on */
    /* from pathsearch.c */
    boolean followup_search;
    FILE *log_file;
    boolean log_opened;                 /* Need to open the log file? */
    /* from progname.c */
    string invocation_name;
    string invocation_short_name;
    string program_name;                /* pretended name */
    int ll_verbose;                     /* for symlinks (conditional) */
    /* from tex-file.c */
    /* If non-NULL, try looking for this if can't find the real font.  */
    const_string fallback_font;
    /* If non-NULL, default list of fallback resolutions comes from this
       instead of the compile-time value.  Set by dvipsk for the R config
       cmd.  *SIZES environment variables override/use as default.  */
    const_string fallback_resolutions_string;
    /* If non-NULL, check these if can't find (within a few percent of) the
       given resolution.  List must end with a zero element.  */
    unsigned *fallback_resolutions;
    kpse_format_info_type format_info[kpse_last_format];
    /* from tex-make.c */
    /* We never throw away stdout, since that is supposed to be the filename
       found, if all is successful.  This variable controls whether stderr
       is thrown away.  */
    boolean make_tex_discard_errors;
    FILE *missfont;
    /* from variable.c  */
    expansion_type *expansions; /* sole variable of this type */
    unsigned expansion_len ;
    /* from xputenv.c */
    /* These record the strings we've set and have to keep around.
       This function can be called many times during a run, and this
       allows us to reclaim memory we allocated.  */
    char **saved_env;           /* keep track of changed items */
    int saved_count;
#if defined(WIN32) || defined(__CYGWIN__)
    char **suffixlist;
#endif /* WIN32 || __CYGWIN__ */

#if defined(WIN32) && !defined(__MINGW32__)
    char the_passwd_name[256];
    char the_passwd_passwd[256];
    char the_passwd_gecos[256];
    char the_passwd_dir[256];
    char the_passwd_shell[256];
    struct passwd the_passwd;
    int __system_allow_multiple_cmds;
#endif /* WIN32 && !__MINGW32__ */
#if defined(WIN32)
    int Is_cp932_system;
    int File_system_codepage;
    int getc_len;
    int getc_buff[4];
    wchar_t wcbuf;
    int st_len;
    char st_buff[5];
    char *st_str;
#endif
} kpathsea_instance;

/* these come from kpathsea.c */
extern KPSEDLL kpathsea kpathsea_new (void) ;
extern KPSEDLL void kpathsea_finish (kpathsea kpse) ;

#if defined (KPSE_COMPAT_API)

#define kpse_bug_address kpathsea_bug_address

extern KPSEDLL kpathsea_instance kpse_def_inst;
extern KPSEDLL kpathsea kpse_def;

#define kpathsea_debug               kpse_def_inst.debug
#define kpse_program_name            kpse_def_inst.program_name
#define kpse_record_input            kpse_def_inst.record_input
#define kpse_record_output           kpse_def_inst.record_output
#define kpse_make_tex_discard_errors kpse_def_inst.make_tex_discard_errors
#define kpse_fallback_font           kpse_def_inst.fallback_font
#define kpse_fallback_resolutions_string  kpse_def_inst.fallback_resolutions_string
#define kpse_fallback_resolutions    kpse_def_inst.fallback_resolutions
#define kpse_format_info             kpse_def_inst.format_info
#define kpse_debug_hash_lookup_int   kpse_def_inst.debug_hash_lookup_int

#undef kpse_invocation_name
#define kpse_invocation_name         kpse_def_inst.invocation_name
#undef kpse_invocation_short_name
#define kpse_invocation_short_name   kpse_def_inst.invocation_short_name

#endif /* KPSE_COMPAT_API */

#ifdef __cplusplus
}
#endif

#endif /* not KPATHSEA_TYPES_H */
