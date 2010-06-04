ChangeLog.TL: TeX Live (TL) changes for ptexenc
===============================================

2010-05-29  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* Makefile.am (nobase_nodist_include_HEADERS): Removed,
	don't install ptexenc/c-auto.h.

2010-05-20  Peter Breitenlohner  <peb@mppmu.mpg.de>

	Moved headers (except jisx0208.h) to ptexenc/, removed patches/.
	* Makefile.am, configure.ac: Adjusted.

2010-04-22  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* ptexenc.[ch] (input_line3): Removed.
	* ptexenc.h (inputline3): Removed.
	(inputline2): Moved from here to ../web2c/ptexdir/kanji.h.

2010-04-19  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* jisx0208.h (new): Copy of ../../libs/gd/gd-2.0.35/jisx0208.h.
	* Makefile.am, unicode-jp.c: Adjusted.

2010-04-16  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* configure.ac: Version 1.1.1 (dev).

2010-04-15  Peter Breitenlohner  <peb@mppmu.mpg.de>

	Avoid compiler warnings.
	* ptexenc.[ch]: Declare ptexenc_version_string, get_enc_string,
	and set_enc_string's params as const.
	* ptexenc.c (enc_to_string): Declare return value as const.
	(string_to_enc): Declare param as const.
	(in_filter): Declare as const.
	(getc4, ungetc4): Declare as static.
	(get_terminal_enc): Declare as static and string var s as const.
	* unicode.c (UCStoUTF8S): Skip unused function.

2010-04-14  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* Makefile.am (rebuild): New target for forced (re-)build.
	(INCLUDES): Add $(KPATHSEA_INCLUDES) in case we use an installed
	(system) version.
	Add rule to rebuild libkpathsea, if necessary.

2010-04-12  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* ptexenc.c (NOFILE) [MinGW32]: Always define it.
