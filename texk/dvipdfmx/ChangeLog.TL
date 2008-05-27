ChangeLog.TL: Changes for dvipdfmx (teTeX / TeX live)
=====================================================

2008-04-06  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* configure.in: respect library dependencies.

2008-03-04  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* Makefile.am: added `ACLOCAL_AMFLAGS -I m4'
	* configure.in: moved macro definitions ...
	* m4/maw-time.m4 (new): ... to here
	* acinclude.m4 (removed): moved macro definitions ...
	* m4/maw-kpse.m4, maw-libs.m4 (both new): ... to here; these
	files are the same as in ../xdvipdfmx.
