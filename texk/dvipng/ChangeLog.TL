	Changes for TeXlive.

2008-05-01  Vladimir Volovich  <vvv@vsu.ru>

	* misc.c: #include dvipng.h again at the top, since the mmap
	clash is resolved.

2008-04-29  Vladimir Volovich  <vvv@vsu.ru>

	* misc.c: #include dvipng.h after sys/mman.h, for AIX.

2008-04-06  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* configure.ac: respect library dependencies.

2008-04-02  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* configure.ac: fix (system and included) library dependencies.

2008-03-31  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* special.c: allow for libgd without jpeg support.
	* configure.ac, config.h.in, configure: adapt.

2008-02-20  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* configure.ac: Use AC_SEARCH_LIBS such that -lm and -lgen are
	automatically appended to LIBS if needed.
	Quote first arg of of AC_DEFUN.

2006-12-24  Karl Berry  <karl@tug.org>

	* dvipng.h: don't try stdbool if we have kpathsea, Staszek's old
	machine doesn't define false.

2006-12-11  Karl Berry  <karl@tug.org>

	* Makefile.in (MKINSTALLDIRS): is in $(srcdir).

2006-12-09  Karl Berry  <karl@tug.org>

	* dvipng.h (alloca) [_AIX]: #define as _alloca, per the autoconf
	manual.

2006-12-05  Karl Berry  <karl@tug.org>

	* Imported release 1.9.

