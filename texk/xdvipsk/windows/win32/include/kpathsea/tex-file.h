/* tex-file.h: find files in a particular format.

   Copyright 1993, 1994, 1995, 1996, 2007, 2008, 2009, 2010, 2013,
   2014 Karl Berry.
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

#ifndef KPATHSEA_TEX_FILE_H
#define KPATHSEA_TEX_FILE_H

#include <kpathsea/c-proto.h>
#include <stdarg.h>
#include <kpathsea/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef MAKE_KPSE_DLL /* libkpathsea internal only */

/* This initializes the fallback resolution list.  If ENVVAR
   is set, it is used; otherwise, the envvar `TEXSIZES' is looked at; if
   that's not set either, a compile-time default is used.  */
extern void kpathsea_init_fallback_resolutions (kpathsea kpse, string envvar);

#endif /* MAKE_KPSE_DLL */

/* If LEVEL is >= FMT's `program_enable_level', set `program_enabled_p'
   for FMT to VALUE.  */
extern KPSEDLL void kpathsea_set_program_enabled (kpathsea kpse,
    kpse_file_format_type fmt, boolean value, kpse_src_type level);

/* Call kpse_set_program_enabled with VALUE and the format corresponding
   to FMTNAME.  */
extern KPSEDLL void kpathsea_maketex_option (kpathsea kpse,
    const_string fmtname, boolean value);

/* Change the list of searched suffixes for FORMAT to ... (alternate
   suffixes if ALTERNATE is true).  */
extern KPSEDLL void kpathsea_set_suffixes (kpathsea kpse,
    kpse_file_format_type format, boolean alternate, ...);

/* Initialize the info for the given format, returning the final search
   path.  This is called automatically by `kpse_find_file', but the
   glyph searching (for example) can't use that function, so it
   must also be available separately.  */
extern KPSEDLL const_string kpathsea_init_format (kpathsea kpse,
    kpse_file_format_type format);

/* Like kpathsea_init_format, but return the list of (environment/config)
   variable names considered, which is not otherwise saved.  This is
   only used by kpsewhich --help.  */
extern KPSEDLL const_string kpathsea_init_format_return_varlist (kpathsea kpse,
  kpse_file_format_type format);

/* If FORMAT has a non-null `suffix' member, append it to NAME "."
   and call `kpse_path_search' with the result and the other arguments.
   If that fails, try just NAME.  */
extern KPSEDLL string kpathsea_find_file (kpathsea kpse, const_string name,
    kpse_file_format_type format,  boolean must_exist);

/* Ditto, allowing ALL parameter and hence returning a NULL-terminated
   list of results.  */
extern KPSEDLL string *kpathsea_find_file_generic (kpathsea kpse,
     const_string name, kpse_file_format_type format, boolean must_exist,
     boolean all);

/* Return true if FNAME is acceptable to open for reading or writing.
   If not acceptable, write a message to stderr.  */
extern KPSEDLL boolean kpathsea_in_name_ok (kpathsea kpse, const_string fname);
extern KPSEDLL boolean kpathsea_out_name_ok (kpathsea kpse, const_string fname);

/* As above, but no error message.  */
extern KPSEDLL boolean kpathsea_in_name_ok_silent
   (kpathsea kpse, const_string fname);
extern KPSEDLL boolean kpathsea_out_name_ok_silent
   (kpathsea kpse, const_string fname);

/* Don't just look up the name, actually open the file.  */
extern KPSEDLL FILE *kpathsea_open_file (kpathsea kpse, const_string name,
                                         kpse_file_format_type format);

/* This function is used to set kpse_program_name (from progname.c) to
   a different value.  It will clear the path searching information, to
   ensure that the search paths are appropriate to the new name. */
extern KPSEDLL void kpathsea_reset_program_name (kpathsea kpse,
                                                 const_string progname);


#if defined (KPSE_COMPAT_API)

extern void kpse_init_fallback_resolutions (string envvar);

extern KPSEDLL void kpse_set_program_enabled (kpse_file_format_type fmt,
                                         boolean value, kpse_src_type level);

extern KPSEDLL void kpse_maketex_option (const_string fmtname,  boolean value);

extern KPSEDLL void kpse_set_suffixes (kpse_file_format_type format,
                                       boolean alternate, ...);

extern KPSEDLL const_string kpse_init_format (kpse_file_format_type);

extern KPSEDLL string kpse_find_file (const_string name,
                            kpse_file_format_type format,  boolean must_exist);

extern KPSEDLL string *kpse_find_file_generic
  (const_string name, kpse_file_format_type format,
      boolean must_exist, boolean all);

extern KPSEDLL boolean kpse_in_name_ok (const_string fname);
extern KPSEDLL boolean kpse_out_name_ok (const_string fname);

/* Here are some abbreviations.  */
#define kpse_find_mf(name)   kpse_find_file (name, kpse_mf_format, true)
#define kpse_find_mft(name)  kpse_find_file (name, kpse_mft_format, true)
#define kpse_find_pict(name) kpse_find_file (name, kpse_pict_format, true)
#define kpse_find_tex(name)  kpse_find_file (name, kpse_tex_format, true)
#define kpse_find_tfm(name)  kpse_find_file (name, kpse_tfm_format, true)
#define kpse_find_ofm(name)  kpse_find_file (name, kpse_ofm_format, true)

/* The `false' is correct for DVI translators, which should clearly not
   require vf files for every font (e.g., cmr10.vf).  But it's wrong for
   VF translators, such as vftovp.  */
#define kpse_find_vf(name) kpse_find_file (name, kpse_vf_format, false)
#define kpse_find_ovf(name) kpse_find_file (name, kpse_ovf_format, false)

extern KPSEDLL FILE *kpse_open_file (const_string, kpse_file_format_type);

extern KPSEDLL void kpse_reset_program_name (const_string progname);
#endif

#ifdef __cplusplus
}
#endif

#endif /* not KPATHSEA_TEX_FILE_H */
