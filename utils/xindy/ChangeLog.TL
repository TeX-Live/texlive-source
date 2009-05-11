ChangeLog.TL: Changes for xindy (teTeX / TeX Live)
======================================================

2009-05-09  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* m4/ac_prog_latex.m4, m4/ac_prog_pdflatex.m4: Use TL_MSG_ERROR
	instead of AC_MSG_ERROR.	

2009-05-09  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* configure.ac: Remove '--with-clisp=...', use 'CLISP=...'.

2009-05-08  Peter Breitenlohner  <peb@mppmu.mpg.de>

	Allow parallel builds:
	* binaries/Makefile.am: Add '.NOTPARALLEL:'.
	* rte/Makefile.am: Serialize rule that creates two targets.

	* configure.ac (AC_CONFIG_AUX_DIR): Use auxiliary files from
	../../build-aux in TeX Live tree.


2009-05-07  Peter Breitenlohner  <peb@mppmu.mpg.de>

	Adapt to TL2009 build system.
	* configure.ac, Makefile.am: Adapt.
	* ac/xindy.ac (new): Configure options also shown at top-level.
	* m4/tl_msg_error.m4 (new): Define TL_MSG_ERROR such that
	'configure --disable-build' succeeds without clisp.

