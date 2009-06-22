ChangeLog.TL: TeX Live (TL) changes for xdvipdfmx
=================================================

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

