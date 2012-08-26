2012-08-26  Akira Kakuto <kakuto@fuk.kindai.ac.jp>

	* tt_table.c (tt_read_hhea_table): typo.

2012-08-23  Khaled Hosny  <khaledhosny@eglug.org>

	* src/Makefile.am: Remove mention of non-existent files.

2012-08-21  Akira Kakuto <kakuto@fuk.kindai.ac.jp>

	* src/tt_post.c (read_v2_post_names): support large truetype fonts
	which are strictly speaking out of spec, idx > 32767.
	* src/dvipdfmx.c: support monocase file system.

2012-08-21  keiichirou shikano <k16.shikano@gmail.com>

	* src/cff.c, src/cff.h. src/type1.c: fix type1 SEAC bug.

2012-08-20  Khaled Hosny  <khaledhosny@eglug.org>

	* src/dvipdfmx.c, src/mem.c, src/mem.h, src/xbb.c, src/dvipdfmx.c,
	src/mem.c, src/mem.h, src/xbb.c: Remove memory debugging code.

2012-08-18  Khaled Hosny  <khaledhosny@eglug.org>

	Move contents of dvipdfmx-20120420 to the top directory
	* Makefile.am, configure.ac, data/Makefile.am, man/Makefile.am,
	src/Makefile.am: Adapted.
	* configure.in, m4/cho-kpse.m4, m4/cho-libs.m4, m4/maw-time.m4:
	Removed.

2012-04-20  Norbert Preining  <preining@logic.at>

	Imported snapshot 20120420.
	* version.ac, man/dvipdfm.1: Adapted.

2011-12-06  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* Makefile.am: Replace ${prefix}/texmf => $(datarootdir)/texmf.

2011-11-05  Akira Kakuto  <kakuto@fuk.kindai.ac.jp>

	* dvi.c, spc-pdfm.c, tfm.c and vf.c: remove incompatible lines
	with the non-upTeX case and erase #ifdef upTeX.

2011-09-07  Akira Kakuto  <kakuto@fuk.kindai.ac.jp>

	* dvi.c, spc-pdfm.c, tfm.c and vf.c: apply patches by Takuji Tanaka
	in order to support (e)-upTeX. The (e)-upTeX support is enabled by
	defining upTeX. In the case -DupTeX, the dvipdfmx must be compatible
	with the old one, which can be obtained without -DupTeX.

2011-06-15  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* version.ac: New file defining the dvipdfmx version.
	* configure.ac: Adapt. (AC_TYPE_UINT32_T) Portability fix for AIX.
	* dpxcrypt.c: Use headers as in configure test for uint32_t.

2011-06-06  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* dpxfile.c (exec_spawn): Do not quote execvp() arguments.

2011-06-03  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* dpxfile.c (exec_spawn): Exit if execvp() fails, some clean up.

2011-06-03  Akira Kakuto  <kakuto@fuk.kindai.ac.jp>

	* dpxfile.c: fix a bug in exec_spawn().

2011-06-03  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* Makefile.am (mapdata*): Do install the map file, still needed
	in pTeX.

2011-06-01  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* Makefile.am (mapdata*): Don't install the map file.

2011-05-30  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* doc/Makefile.am: Use ../am/bin_links.am for $(bindir) links.

2011-05-03  Peter Breitenlohner  <peb@mppmu.mpg.de>

	Imported snapshot 20110311.
	* configure.ac: Adapted.

2011-03-09  Peter Breitenlohner  <peb@mppmu.mpg.de>

	Imported snapshot 20110309.
	* configure.ac: Adapted.

2011-03-08  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* configure.ac: Check for sys/wait.h, in order to define
	HAVE_SYS_WAIT_H (required for src/dpxfile.c).

2011-03-07  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* Makefile.am, configure.ac: Proxy build system for TeX Live,
	using the dvipdfmx-20110305 distribution tree.
	* dvipdfmx-20110305-PATCHES: New directory with patches applied
	to the dvipdfmx-20110305 distribution.

2011-02-28  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* src/pngimage.c (create_cspace_ICCBased): Avoid libpng-1.5.x
	compiler warning.

2011-02-15  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* src/Makefile.am [WIN32]: Add dvipdfmx.exe, ebb.exe, and
	extractbb.exe wrappers for MinGW.

2010-02-11  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* man: New directory with manual pages (from Master).
	man/dvipdft.1 needs to be updated and man/dvipdfm.1 replaced by
	man/dvipdfmx.1 (with a link to dvipdfm.1).
	* Makefile.am, configure.ac: Adapted.

	* man/extractbb.1, man/dvipdfm.1, dvipdft.1: Formatting.

2010-12-30  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* src/dvipdfmx.c: Check for argv[0] with .exe via strcasecmp().

2010-12-29  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* configure.ac, src/Makefile.am: Ignore shell script for WIN32.

2010-06-10  Peter Breitenlohner  <peb@mppmu.mpg.de>

	Avoid using "%hu" with fscanf/sscanf, for older Solaris.
	* src/pdfobj.c (pdf_open, check_for_pdf_version): Declare minor
	as unsigned int for use with "%u".

2010-06-08  Peter Breitenlohner  <peb@mppmu.mpg.de>

	Solaris9 doesn't support "%hhu".
	* src/dpxfile.c (dpx_file_apply_filter):
	Use (unsigned short) version with "%hu".
	* src/pdfobj.c (pdf_open, check_for_pdf_version): Declare minor
	as unsigned short for use with "%hu".

2010-03-29  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* configure.ac: Remove AC_TYPE_SIZE_T, now part of KPSE_COMMON.

2010-02-22  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* src/pngimage.c (check_for_png): Replace png_check_sig() by
	png_sig_cmp() for libpng-1.4.
	From Hanno Boeck <hanno@hboeck,de>.

2010-02-20  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* src/sfnt.h: #define CHAR as SFNT_CHAR to avoid conflict with
	CHAR from <winnt.h>.

2009-11-07  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* configure.ac, src/dpxfile.c: Require recent kpathsea version.

2009-09-02  Peter Breitenlohner  <peb@mppmu.mpg.de>

	Avoid more maximal compiler warnings:
	* src/pst_obj.c (pst_new_mark, pst_release_obj, pst_parse_null):
	Instead of casting a const string to non-const, make a copy and
	later free the copy.

2009-08-31  Peter Breitenlohner  <peb@mppmu.mpg.de>

	Avoid almost all maximal compiler warnings:
	src/agl.c (agl_load_listfile),
	src/cid.c (CIDFont_base_open),
	src/cff_types.h (cff_dict_entry),
	src/cff_dict.c (add_dict, cff_dict_add),
	src/dpxutil.[ch] (read_c_escchar, read_c_litstrc,
	parse_c_string, parse_c_ident, parse_float_decimal),
	src/dvi.c (dvi_do_special, read_length, scan_special),
	src/dvipdfmx.c (read_length, select_paper, select_pages,
	do_args, read_config_file),
	src/fontmap.c (skip_blank, parse_string_value,
	parse_integer_value, fontmap_parse_mapdef_dpm,
	fontmap_parse_mapdef_dps, pdf_read_fontmap_line,
	is_pdfm_mapline, pdf_load_fontmap_file),
	src/mpost.[ch] (mps_scan_bbox, skip_prolog, do_mpost_bind_def,
	mp_parse_body, mps_exec_inline, mps_include_page, mps_do_page),
	src/otl_conf.c (parse_uc_coverage, parse_substrule, parse_block),
	src/otl_opt.c (parse_expr, otl_parse_optstring),
	src/pdfdev.c (handle_multibyte_string, pdf_dev_set_string),
	src/pdfdoc.c (name_dict, pdf_doc_init_names),
	src/pdfencoding.c (load_encoding_file),
	src/pdfobj.c (find_xref, parse_trailer, pdf_read_object,
	read_objstm, pdf_get_object),
	src/pdfparse.[ch] (save, skip_line, skip_white, parse_number,
	parse_unsigned, parse_gen_ident, parse_ident, parse_val_ident,
	parse_opt_ident, parse_pdf_number, pn_getc, parse_pdf_name,
	parse_pdf_boolean, parse_pdf_null, ps_getescc,
	parse_pdf_literal_string, parse_pdf_hex_string,
	parse_pdf_string, parse_pdf_tainted_dict, parse_pdf_dict,
	parse_pdf_array, parse_pdf_stream, parse_pdf_reference,
	try_pdf_reference, parse_pdf_object),
	src/spc_color.c (skip_blank, spc_color_check_special,
	spc_color_setup_handler),
	spc_dvips.c (parse_filename, spc_dvips_check_special,
	spc_dvips_setup_handler),
	src/spc_html.c (parse_key_val, read_html_tag, atopt,
	spc_html__img_empty, cvt_a_to_tmatrix, spc_html_check_special,
	spc_html_setup_handler),
	src/spc_misc.c (spc_handler_postscriptbox,
	spc_misc_check_special, spc_misc_setup_handler),
	src/spc_pdfm.c (my_parse_pdf_dict, spc_pdfm_check_special,
	spc_pdfm_setup_handler),
	src/spc_tpic.c (skip_blank, spc_tpic_check_special,
	spc_tpic_setup_handler),
	src/spc_util.c (skip_blank),
	src/specials.c (init_special, print_error),
	src/specials.h (spc_arg), src/subfont.c (read_sfd_record):
	Rewrite all parsing routines to operate on const strings.
	Constify start/end pointer args and local vars.
	Constify struct member strings.  Drop or adjust casts.
	Cast a few non-const start pointers to 'const char **'.
	Use a few non-const local vars, not to modify const strings.

2009-08-30  Peter Breitenlohner  <peb@mppmu.mpg.de>

	Avoid some more maximal compiler warnings.

	* src/tt_post.[ch] (tt_post_table): Constify struct member
	strings and drop casts.
	(macglyphorder): Declare 'const char**' not 'const char*const*'.

	* src/truetype.c (selectglyph): Use additional const string local
	var and drop cast.

	* src/dpxcrypt.[ch] (transform, MD5_write): Constify args.
	* src/pdfencrypt.c (compute_user_password): Drop cast.

	* src/fontmap.c (chop_sfd_name): Drop useless cast.
	(substr): Constify string arg and local var.
	(strip_options): Constify local var and drop cast.

	* src/cmap_write.c (CMap_create_stream): Constify cast.
	* src/pdfcolor.c (str2iccSig, iccp_check_colorspace,
	iccp_get_rendering_intent, iccp_unpack_header,
	iccp_get_checksum): Constify local vars and casts.
	* src/pdfencoding.[ch] (pdf_encoding_new_encoding,
	pdf_encoding_findresource): Constify args.
	* src/pdfobj.c (pdf_out): Constify cast.
	* src/specials.c (spc_exec_special): Drop useless casts.

	* src/pdfdoc.c (pdf_doc_add_goto): Constify local vars.

2009-08-28  Peter Breitenlohner  <peb@mppmu.mpg.de>

	Avoid some more compiler warnings.

	* src/pdfobj.c (parse_xref_stream): Drop inner declaration of
	size and use size_obj, not to shadow previous local (diff type).
	Rename local index=>index_obj, not to shadow string function.

2009-08-27  Peter Breitenlohner  <peb@mppmu.mpg.de>

	Avoid some more maximal compiler warnings.

	* src/cff.[ch] (cff_get_sid, cff_add_string): Constify string arg.
	* src/cidtype2.c (find_tocode_cmap), src/cmap.c (CMap_decode_char),
	src/dpxfile.c (dpx_create_temp_file, dpx_file_apply_filter):
	Constify local vars and drop casts.
	* src/dpxutil.c (get_hash): Constify cast.
	* src/pdfnames.c (cmp_key), src/t1_char.c (stem_compare),
	tt_glyf.c (glyf_cmp): Constify local vars and adjust casts.
	* src/t1_load.[ch] (t1_get_standard_glyph): Constify return
	type and drop cast.
	* src/type1.c (pdf_font_load_type1): Constify local vars.
	* src/cidtype0.c (CIDFont_type0_t1cdofont,
	CIDFont_type0_t1dofont): Drop casts.

2009-08-26  Peter Breitenlohner  <peb@mppmu.mpg.de>

	Avoid some maximal compiler warnings.

	* src/mfileio.c (os_error),
	src/pdfencoding.c (pdf_encoding_complete):
	ANSI C function definitions, i.e., '()'=>'(void)'.

	* src/agl.c (skip_capital, skip_modifier): Constify string args.
	(is_smallcap, agl_name_convert_unicode, put_unicode_glyph,
	agl_sput_UTF16BE, agl_get_unicodes): Constify local strings
	and drop casts.
	* src/xbb.c (extensions): Constify global strings.

2009-07-17  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* src/pdfximage.h: add prototype for pdf_ximage_get_subtype.

2009-06-20  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* Makefile.am (AM_CFLAGS): enable compiler warnings.
	* src/pdfobj.c (pdf_file_new): declare as static.

2009-06-05  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* configure.ac: replace '--enable-tetex-build' by
	'--enable-texlive-build' (automatic if configured as part of the
	TeXLive tree).

2009-06-04  Karl Berry  <karl@tug.org>

	* src/Makefile.am (dist_bin_SCRIPTS): dvipdft.
	* src/dvipdft: revised script from Matthias to replace
	version in dvipdfm.

2009-05-27  Peter Breitenlohner  <peb@mppmu.mpg.de>

	Adapt to TL2009 build system.

2009-05-15  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* pdfdoc.c (pdf_doc_get_page): Avoid C99isms
	(variable declaration not at the beginning of a block).

	From  Vladimir Volovich <vvv@vsu.ru>

2009-03-16  Peter Breitenlohner  <peb@mppmu.mpg.de>
	* src/cff.c, src/dpxutil.c, src/mem.c, src/pdfencoding.c,
	src/pdfencrypt.c: Must include Kpathsea headers (via `system.h')
	before including `mem.h'.

2008-04-06  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* configure.in: respect library dependencies.

2008-03-04  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* Makefile.am: added `ACLOCAL_AMFLAGS -I m4'
	* configure.in: moved macro definitions ...
	* m4/maw-time.m4 (new): ... to here
	* acinclude.m4 (removed): moved macro definitions ...
	* m4/maw-kpse.m4, maw-libs.m4 (both new): ... to here; these
	files are the same as in ../xdvipdfmx.
