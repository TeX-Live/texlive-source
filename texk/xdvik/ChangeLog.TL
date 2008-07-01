ChangeLog: Changes for xdvik (teTeX / TeX live)
===============================================

2008-07-01
	Imported ftp://tug.org/tex/xdvik-22.84.14.tar.gz and
	adapted to new TeX live build system (autoconf-2.59+).

	* Makefile.in.orig, acconfig.h.orig, aclocal.m4.orig,
	configure.in.orig, depend.mk.orig, gui/Makefile.in.orig,
	gui/depend.mk.orig, kpathsea.ac.orig, tests/Makefile.in.orig,
	tests/depend.mk.orig, withenable.ac.orig (all new):
	Original files from xdvik-22.84.14.

	* Makefile.in, gui/Makefile.in, tests/Makefile.in:
	Added '@configure_input@' line and replace ac_include by
	kpse_include.

	* acinclude.m4 (new): Renamed from aclocal.m4.
	Renamed cache variables (must contain '_cv_').

	* acinclude.m4, configure.in, withenable.ac:
	Added lots of m4 quoting and third args of AC_DEFINE.

	* acconfig.h (removed): Use AC_DEFINE with three args to
	provide templates.

	* configure.in:
	Replaced AC_OUTPUT with arguments by KPSE_CONFIG_FILES,
	AC_CONFIG_FILES and AC_OUTPUT without arg.

	* kpathsea.ac (LIBKPATHSEACPPFLAGS): Set to '-I$(kpathsea_parent)'
	instead of '-I$(kpathsea_dir)' to avoid confusion between
	<c-auto.h> and <kpathsea/c-auto.h>.

	* depend.mk, gui/depend.mk, tests/depend.mk: Regenerated and
	cleaned by hand ('make depend' didn't quite do that). 

	* aclocal.m4, c-auto.in, configure: Regenerated.

2008-02-29  Peter Breitenlohner  <peb@mppmu.mpg.de>
	* acinclude.m4: added quotes to DEFUN's first args, added
	AC_DEFINES's third arg to provide autoheader templates ...
	* configure.in, withenable.ac: added AC_DEFINES's third arg to provide
	autoheader templates ...
	* acconfig.h: ... removed.

2008-01-31  Peter Breitenlohner  <peb@mppmu.mpg.de>
	Convert from autoconf-2.13 to autoconf-2.59+:
	* configure.in (AC_PREREQ): Require autoconf-2.59.
	* Makefile.in, gui/Makefile.in: Add @configure_input@ lines.
	* acinclude.m4 (new): Renamed from aclocal.m4.
	* aclocal.m4 (new): Generated (aclocal -I ../m4).
	* ../../libs/t1lib/t1lib.ac: Fixed a typo.
	* configure: Regenerated (autoconf).
