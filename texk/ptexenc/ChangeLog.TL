ChangeLog.TL: TeX Live (TL) changes for ptexenc
===============================================

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
