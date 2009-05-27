ChangeLog.TL: Changes for dvipdfmx (teTeX / TeX Live)
=====================================================

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
