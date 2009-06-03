	Changes for teTeX / TeX Live.

2009-03-25  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* dvipng-1.12-patches (new): patches applied to dvipng-1.12.

2009-03-25  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* color.c, dvipng.h, fontmap.c, ft.c, misc.c, pk.c, set.c,
	t1.c, tfm.c: "#if HAVE_ALLOCA_H" => "#ifdef HAVE_ALLOCA_H"
	and similar, to avoid preprocessor warnings.

2009-03-24  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* Imported release 1.12 from savannah.nongnu.org.
	* *-1.11 (removed), *-1.12 (new): Original files.

	* Makefile.am, configure.ac: New TeX Live build system.

	From Vladimir Volovich  <vvv@vsu.ru>
	* special.c: portability fix - put variable declarations
	at the beginning of the block.

2008-05-15  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* Imported release 1.11 from savannah.nongnu.org.
	* *-1.10 (removed), *-1.11 (new): Original files.
	* configure.ac: Updated for release 1.11.

	From Vladimir Volovich  <vvv@vsu.ru>
	* misc.c: fix the field name: mmap => data (oversight from
	old code?)
	* special.c: portability fix - put variable declarations
	at the beginning of the block.

2008-05-14  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* Imported release 1.10 from savannah.nongnu.org.

	* Makefile.in-1.10, aclocal.m4-1.10, config.h.in-1.10,
	configure.ac-1.10, configure-1.10 (all new): Original files
	from release 1.10.

	* acinclude.m4 (new): M4 macros extracted from aclocal.m4-1.10.

	* Makefile.in, configure.ac: Adapted for TeX Live.

	* aclocal.m4, config.h.in, configure: Regenerated.

	* dvipng.{dvi,help,info}: Updated generated files.

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

