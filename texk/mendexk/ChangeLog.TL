ChangeLog.TL: TeX Live (TL) changes for mendexk
===============================================

2010-04-16  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* exvar.h, var.h (atama, page_compositor, page_precedence):
	Change types from 'char' back to 'unsigned char' as they
	originally were in exvar.h.  From Sadayuki Tanaka
	<s-tanaka@asciimw.jp>, 16 Apr 2010 16:30:22.

2010-04-15  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* pageread.c: #include "mendex.h" for prototype.
	* kana.h: #include "exkana.h" for type checking.
	* var.h: #include "exvar.h" for type checking.
	* sort.c (wcomp): Initialize local variable prechar.
	* kp.c (KP_init): Use kpse_set_program_name() because
	kpse_set_progname() is deprecated since ages and no longer
	exported from kpathsea.

	FIXME: Is this correct?
	* exvar.h (atama, page_compositor, page_precedence): Change type
	from 'unsigned char' to 'char' as defined in var.h.

	* convert.c (dicread): Add 'return 0' at end.

	Avoid compiler warnings.
	* ANSI C function prototypes and definitions.
	* Declare functions as static, skip compilation of unused ones.
	* Declare various function param or result, struct member, and
	local or global var strings as const.

2010-04-12  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* Makefile.am (EXTRA_DIST): Add indexsty, mendex.doc.
	Add rule to rebuild kpathsea.
