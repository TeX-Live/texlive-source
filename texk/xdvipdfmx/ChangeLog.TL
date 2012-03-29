ChangeLog.TL: TeX Live (TL) changes for xdvipdfmx
=================================================

2012-03-29  Peter Breitenlohner  <peb@mppmu.mpg.de>

	Remove extern decls from *.c files.

	* dvi.c: Move extern decls from here ...
	* dvi.h: ... to here ...
	* fontmap.h: ... and here.

	* tt_aux.c: Move extern decl from here ...
	* tt_aux.h: ... to here ...
	* dvipdfmx.c: ... and #include "tt_aux.h".

	* dpxfile.c: Drop obsolete comment that mkstemp may not be
	declared in stdlib.h.
	* spc_color.c: Drop 'extern' from function definition.
	* error.h: ANSI C prototype for error_cleanup().
	* error.c: Drop extern decl, now in error.h.
	* dvipdfmx.c: Add '#include "error.h"' and drop extern decl.

2012-03-05  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* pdfdev.c (handle_multibyte_string): Handle UTP-16 surrogates
	for UCS-4 supplementary planes.
	Based on patch from Tanaka Takuji-san.

2012-03-04  Peter Breitenlohner  <peb@mppmu.mpg.de>

	Avoid 'the address of XXX will always evaluate as true' warnings.
	* pdfdraw.c (m_stack_depth): Use '(s) != NULL' instead of '(s)'.
	Gcc >=4.3 produces identical code but -Wall yields no warning.

2012-02-28  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* configure.ac: Fixed a typo (no libpaper for TeX Live builds).

2011-12-06  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* data/Makefile.am:
	Replace ${prefix}/texmf => $(datarootdir)/texmf.

2011-11-06  Akira Kakuto  <kakuto@fuk.kindai.ac.jp>

	* spc_pdfm.c: fix a typo in spc_handler_pdfm_dest().
	reencode also the destination in \special{pdf:dest ...} to UTF-16.

2011-06-15  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* configure.ac (AC_TYPE_UINT32_T): Portability fix for AIX.
	* dpxcrypt.c: Use headers as in configure test for uint32_t.

2011-06-06  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* dpxfile.c (exec_spawn): Do not quote execvp() arguments.

2011-06-03  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* dpxfile.c (exec_spawn): Exit if execvp() fails, some clean up.

2011-06-03  Akira Kakuto  <kakuto@fuk.kindai.ac.jp>

	* dpxfile.c: fix a bug in exec_spawn().

2011-04-18  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* src/fontmap.c [OS X]: #include FT_MAC_H to get prototype for
	FT_GetFilePath_From_Mac_ATS_Name().

2011-03-11  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* src/dpxcrypt.c: Sync with dvipdfmx.

	* src/dpxconf.[ch], src/numbers.[ch], src/pdfresource.[ch],
	src/pst_obj.[ch], src/tfm.[ch], src/tt_table.h: Remove more
	trivial diffs with dvipdfmx (unused functions).

2011-03-10  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* src/cff_dict.h, src/cff_types.h, src/cmap.c, src/cmap_write.c,
	src/dpxcrypt.[ch], src/dvipdfmx.c, src/mfileio.c, src/mpost.h,
	src/otl_conf.c, src/otl_opt.c, src/pdfencoding.[ch],
	src/pdfencrypt.[ch], src/pdflimits.h, src/pdfparse.h,
	src/specials.h, src/subfont.c, src/t1_char.c, src/tt_post.h,
	src/vf.c: Remove trivial diffs with dvipdfmx.

	* src/Makefile.am: Mark asl_charset.h as unused.

2011-03-09  Akira Kakuto  <kakuto@fuk.kindai.ac.jp>

	* src/dvipdfmx.c, src/pdfencrypt.c and src/pdfencrypt.h:
	recover compute_id_string() for the consistency with dvipdfmx
	as much as possible.

2011-03-09  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* configure.ac: Check for <sys/wait.h>, drop check for system().
	* src/pdfencrypt.c (compute_id_string): Drop unused function.

2011-03-09  Akira Kakuto  <kakuto@fuk.kindai.ac.jp>
	* src/dpxfile.c: Avoid to use system().
	* src/dvi.c, src/dvi.h, src/dvipdfmx.c, src/pdfencript.c,
	src/pdfencrypt.h, src/spc_pdfm.c: new special pdf:encrypt
	by Matthias Franz.

2011-02-28  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* src/pngimage.c (create_cspace_ICCBased): Avoid libpng-1.5.x
	compiler warning.

2010-06-08  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* src/dpxfile.c (dpx_file_apply_filter) [Solaris9]:
	Avoid "%hhu", use "%hu" and (unsigned short) version.

2010-03-29  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* configure.ac: Remove AC_TYPE_SIZE_T, now part of KPSE_COMMON.

2010-02-22  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* src/pngimage.c (check_for_png), src/xbb.c (check_for_png):
	Replace png_check_sig() by png_sig_cmp() for libpng-1.4.
	From Hanno Boeck <hanno@hboeck,de>.

2010-02-20  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* configure.ac: Don't fail configure if not building xdvipdfmx.

	* src/sfnt.h: #define CHAR as SFNT_CHAR to avoid conflict with
	CHAR from <winnt.h>.

2009-11-07  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* configure.ac, src/dpxfile.c: Require recent kpathsea version.

2009-09-02  Peter Breitenlohner  <peb@mppmu.mpg.de>

	Avoid more maximal compiler warnings:
	* src/pst_obj.c (pst_new_mark, pst_release_obj, pst_parse_null):
	Instead of casting a const string to non-const, make a copy and
	later free the copy.

2009-08-30  Peter Breitenlohner  <peb@mppmu.mpg.de>

	Avoid almost all maximal compiler warnings:
	* src/agl.c (agl_load_line, agl_load_listfile),
	src/cid.c (CIDFont_base_open),
	src/cff_types.h (cff_dict_entry),
	src/cff_dict.c (add_dict, cff_dict_add),
	src/dpxutil.[ch] (read_c_escchar, read_c_litstrc,
	parse_c_string, parse_c_ident, parse_float_decimal),
	src/dvi.c (dvi_do_special, read_length, scan_special),
	src/dvipdfmx.c (read_length, select_paper, select_pages,
	do_args, read_config_file),
	src/epdf.c (pdf_copy_clip),
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
	spc_dvips.c (parse_filename, check_next_obj,
	spc_handler_ps_tricks_parse_path, spc_handler_ps_tricks_render,
	spc_dvips_check_special, spc_dvips_setup_handler),
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
	src/spc_xtx.c (spc_xtx_check_special, spc_xtx_setup_handler),
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
	(pdf_load_native_font): Constify local var and drop cast.

	* src/cmap_write.c (CMap_create_stream): Constify cast.
	* src/pdfcolor.c (str2iccSig, iccp_check_colorspace,
	iccp_get_rendering_intent, iccp_unpack_header,
	iccp_get_checksum): Constify local vars and casts.
	* src/pdfencoding.[ch] (pdf_encoding_new_encoding,
	pdf_encoding_findresource): Constify args.
	* src/pdfobj.c (pdf_out): Constify cast.
	* src/specials.c (spc_exec_special): Drop useless casts.

	* src/spc_dvips.c (spc_handler_ps_tricks_brotate,
	spc_handler_ps_tricks_transform): Constify local vars.
	(pstricks_key): Constify string members.

2009-08-28  Peter Breitenlohner  <peb@mppmu.mpg.de>

	Avoid some more maximal compiler warnings.

	Avoid zillions of 'initialization discards' warnings.
	src/agl.h, src/agl_standard.c, src/mk_agl_standard_c.pl:
	Constify string array 'agl_standard_names'.
	src/agl.c (agl_load_standard_names): Adjust.

	Some "declaration of 'xxx' shadows ...".  Among the remaining
	ones, those for 'distiller_template' in src/spc_dvips.c (local
	vs. global) should be looked at (FIXME).
	* src/pdfobj.c (parse_xref_stream): Drop inner declaration of
	size and use size_obj, not to shadow previous local (diff type).
	* src/epdf.c (pdf_get_page_content): Drop unnecessary inner
	declaration of content_new, not to shadow previous local.

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

	* src/agl.c (agl_load_standard_names),
	src/dvi.c (get_buffered_unsigned_byte, get_buffered_signed_byte,
	get_buffered_unsigned_pair, get_buffered_signed_pair,
	get_buffered_unsigned_triple, get_buffered_signed_triple,
	get_buffered_signed_quad, get_buffered_unsigned_quad,
	do_pic_file),
	src/mfileio.c (os_error),
	src/pdfencoding.c (pdf_encoding_complete):
	ANSI C function definitions, i.e., '()'=>'(void)'.

	* src/agl.c (skip_capital, skip_modifier): Constify string args.
	(is_smallcap, agl_name_convert_unicode, put_unicode_glyph,
	agl_sput_UTF16BE, agl_get_unicodes): Constify local strings
	and drop casts.
	* src/xbb.c (extensions, xbb_file_mode): Constify global strings.

2009-06-20  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* Makefile.am (AM_CFLAGS): enable compiler warnings.
	* src/agl.c (agl_load_line): delare as static, add forward decl.
	* src/cidtype0.c (CIDFont_type0_t1open): remove ft_to_gid (unused).
	* src/dvi.c (do_xxx): avoid unused variable.
	(do_pic_file): declare as static.
	(get_and_buffer_signed_byte): unused.
	* src/dvipdfmx.c: remove do_objstm (unused).
	* src/epdf.c: #include "pdfdoc.h" for pdf_doc_add_page_content.
	(pdf_get_page_obj): remove info, contents, contents_dict, and
	matrix (all unused).
	(pdf_include_page): remove page_idx (unused).
	(pdf_copy_clip): remove p0, p1, p2, and bbox (all unused),
	silence initialized warning.
	* src/jpegimage.c (read_APP1_Exif): remove rational_value (unused),
	silence initialized warnings.
	* src/pdfdev.c: #include "dvi.h" for dvi_is_tracking_boxes.
	* src/pdfobj.c (pdf_file_new): declare as static.
	* src/spc_dvips.c: #include "epdf.h" for pdf_copy_clip.
	(spc_handler_ps_tricks_gdef): unused.
	(spc_dvips_at_begin_page): remove fp (unused).
	* src/tt_aux.[ch] (tt_aux_set_verbose): use ANSI C prototype.
	* src/tt_cmap.c (handle_subst_glyphs): remove ucv (unused).

2009-06-05  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* configure.ac: replace '--enable-tetex-build' by
	'--enable-texlive-build' (automatic if configured as part of the
	TeXLive tree).

2009-05-27  Peter Breitenlohner  <peb@mppmu.mpg.de>

	Adapt to TL2009 build system.

2009-05-25  Peter Breitenlohner  <peb@mppmu.mpg.de>
	* configure.ac, src/Makefile.am: Do not use CPPFLAGS for
	required -I flags, leave it for the user.

2009-03-16  Peter Breitenlohner  <peb@mppmu.mpg.de>
	* src/cff.c, src/dpxutil.c, src/mem.c, src/pdfencoding.c,
	src/pdfencrypt.c: Must include Kpathsea headers (via `system.h')
	before including `mem.h'.

2008-04-06  Peter Breitenlohner  <peb@mppmu.mpg.de>
	* configure.in: respect library dependencies.

2008-03-31  Peter Breitenlohner  <peb@mppmu.mpg.de>
	* Makefile.in, data/Makefile.in, src/Makefile.in: regenerated.

2008-03-04  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* Makefile.am: added `ACLOCAL_AMFLAGS -I m4'
	* configure.in: moved macro definitions ...
	* m4/maw-time.m4 (new): ... to here
	* acinclude.m4 (removed): moved macro definitions ...
	* m4/maw-kpse.m4, maw-libs.m4, maw-macos.m4 (all new): ... to
	here; these files are the same as in ../dvipdfmx.

2008-02-21  Peter Breitenlohner  <peb@mppmu.mpg.de>

	Allow maintainer mode to regenerate files.
	* configure.in, acinclude.m4: added AC_DEFINES's third arg to
	provide autoheader templates.
	* configure.in: use AC_DEFINES for XETEX and XETEX_MAC.

