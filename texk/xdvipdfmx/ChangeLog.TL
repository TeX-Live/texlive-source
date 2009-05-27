ChangeLog.TL: Changes for xdvipdfmx (teTeX / TeX Live)
======================================================

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

