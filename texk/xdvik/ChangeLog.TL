ChangeLog: Changes for xdvik (teTeX / TeX live)
===============================================

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
