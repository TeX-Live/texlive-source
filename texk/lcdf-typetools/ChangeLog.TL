ChangeLog.TL: TeX Live (TL) changes for lcdf-typetools
======================================================

2010-02-21  Peter Breitenlohner  <peb@mppmu.mpg.de>

	Various MinGW32 (cross) build fixes.
	* configure.ac, include/efont/otfdata.hh:
	MinGW32 has <sys/param.h> but also needs <winsock2.h>.
	MinGW32 uses <windows.h> instead of <win32lib.h>.
	* configure.ac (LIBS) [WIN32]: Add -lwsock32.

	* t1rawafm/t1rawafm.cc: Do not define away 'std'.
	* t1testpage/t1testpage.cc: Do not define away 'std'.
	* ttftotype42/ttftotype42.cc (do_file): Remove 'if(binary)'.

2010-02-20  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* configure.ac: Don't fail configure if not building lcdf-typetools.

2009-09-03  Karl Berry  <karl@tug.org>

	* configure.ac: AC_DEFINE(HAVE_KPATHSEA) if we have it, else
	kpse stuff not really enabled, e.g., glyphlist.txt and
	texnansx.enc not found
	tlbuild mail from Jown Owens, 31 Aug 2009 14:34:39.

2009-06-19  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* configure.ac: enable compiler warnings (TeX Live style).
	* liblcdf/clp.c (Clp_OptionChar, finish_string_list): declare
	as static.

2009-04-22  Peter Breitenlohner  <peb@mppmu.mpg.de>

	Adapt to TL2009 build system.

