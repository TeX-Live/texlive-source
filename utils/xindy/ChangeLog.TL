ChangeLog.TL: Changes for xindy (teTeX / TeX Live)
======================================================

2009-05-08  Peter Breitenlohner  <peb@mppmu.mpg.de>

	Adapt to TL2009 build system.
	* configure.ac, Makefile.am: Adapt.
	* ac/xindy.ac (new): Configure options also shown at top-level.
	* m4/tl_msg_error.m4 (new): Define TL_MSG_ERROR such that
	'configure --disable-build' succeeds without clisp.

	Allow parallel builds:
	* binaries/Makefile.am: Add '.NOTPARALLEL:'.
	* rte/Makefile.am: Serialize rule that creates two targets.

	* configure.ac: Remove '--with-clisp=...', use 'CLISP=...'.

