/* kpseinterface.{c,h} -- interface with the kpathsea library
 *
 * Copyright (c) 2003-2011 Eddie Kohler
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version. This program is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details.
 */

/* The W32TEX version does not link to Kpathsea, but uses the DLL
 * built by MSVC.
 */

#if !defined(W32TEX)
#include <kpathsea/config.h>
#endif
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#ifdef W32TEX
#define ENV_SEP ';'
#include <windows.h>
extern HINSTANCE Hinstance;
#else
#include <kpathsea/progname.h>
#include <kpathsea/expand.h>
#include <kpathsea/c-pathch.h>
#include <kpathsea/tex-file.h>
#endif
#include "kpseinterface.h"

int kpsei_env_sep_char = ENV_SEP;

#ifdef W32TEX
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
#endif

void
kpsei_init(const char* argv0, const char* progname)
{
#ifdef W32TEX
  PROC pkpse_set_program_name;
  pkpse_set_program_name = GetProcAddress(Hinstance, "kpse_set_program_name");
  if(!pkpse_set_program_name) {
    fprintf(stderr, "Failed to find Kpathsea function.\n");
    return;
  }
  (void)pkpse_set_program_name(argv0, progname);
#else
    kpse_set_program_name(argv0, progname);
#endif
#ifdef SELFAUTODIR
    putenv("SELFAUTODIR=" SELFAUTODIR);
#endif
#ifdef SELFAUTOLOC
    putenv("SELFAUTOLOC=" SELFAUTOLOC);
#endif
#ifdef SELFAUTOPARENT
    putenv("SELFAUTOPARENT=" SELFAUTOPARENT);
#endif
}

char*
kpsei_path_expand(const char* path)
{
#ifdef W32TEX
  PROC pkpse_path_expand;
  pkpse_path_expand = GetProcAddress(Hinstance, "kpse_path_expand");
  if(!pkpse_path_expand) {
    fprintf(stderr, "Failed to find Kpathsea function.\n");
    return 0;
  }
  return (char *)pkpse_path_expand(path);
#else
    return kpse_path_expand(path);
#endif
}

#ifdef W32TEX
#define kpse_find_file  (char *)pkpse_find_file
#endif

char*
kpsei_find_file(const char* name, int format)
{
#ifdef W32TEX
  PROC pkpse_find_file;
  pkpse_find_file = GetProcAddress(Hinstance, "kpse_find_file");
  if(!pkpse_find_file) {
    fprintf(stderr, "Failed to find Kpathsea function.\n");
    return 0;
  }
#endif
    switch (format) {
      case KPSEI_FMT_WEB2C:
	return kpse_find_file(name, kpse_web2c_format, true);
      case KPSEI_FMT_ENCODING:
	return kpse_find_file(name, kpse_enc_format, true);
      case KPSEI_FMT_TYPE1:
	return kpse_find_file(name, kpse_type1_format, false);
      case KPSEI_FMT_TYPE42:
	return kpse_find_file(name, kpse_type42_format, false);
      case KPSEI_FMT_TRUETYPE:
	return kpse_find_file(name, kpse_truetype_format, false);
#if HAVE_DECL_KPSE_OPENTYPE_FORMAT
      case KPSEI_FMT_OPENTYPE:
	return kpse_find_file(name, kpse_opentype_format, false);
#endif
      case KPSEI_FMT_OTHER_TEXT:
	return kpse_find_file(name, kpse_program_text_format, true);
      case KPSEI_FMT_MAP:
	return kpse_find_file(name, kpse_fontmap_format, true);
      default:
	return 0;
    }
}

void
kpsei_set_debug_flags(unsigned flags)
{
#ifdef W32TEX
  unsigned int *pkpathsea_debug;
  pkpathsea_debug = (unsigned int *)GetProcAddress(Hinstance, "kpathsea_debug");
  if(!pkpathsea_debug) {
    fprintf(stderr, "Failed to find Kpathsea variable.\n");
    return;
  }
  *pkpathsea_debug = flags;
#else
    kpathsea_debug = flags;
#endif
}

#ifdef W32TEX
int
kpsei_getlongpath(char *buff, char *input, int len)
{
  PROC pkpse_getlongpath;
  pkpse_getlongpath = GetProcAddress(Hinstance, "getlongpath");
  if(!pkpse_getlongpath) {
    fprintf(stderr, "Failed to find Kpathsea function.\n");
    return 0;
  }
  return (int)pkpse_getlongpath(buff, input, len);
}

char *
kpsei_var_value(const char *var)
{
  PROC pkpse_var_value;
  pkpse_var_value = GetProcAddress(Hinstance, "kpse_var_value");
  if(!pkpse_var_value) {
    fprintf(stderr, "Failed to find Kpathsea function.\n");
    return NULL;
  }
  return (char *)pkpse_var_value(var);
}
#endif
