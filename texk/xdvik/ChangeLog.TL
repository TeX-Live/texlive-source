ChangeLog.TL: Changes for xdvik (TeX Live)
==================================================

2012-04-18  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* configure.ac: Remove some obsolete comments.

	* m4/xdvi-check-iconv.m4, m4/xdvi-check-langinfo.m4,
	m4/xdvi-func-working-vsnprintf.m4: Update copyright statements
	with info provided by Paul Vojta <vojta@math.berkeley.edu>.

2012-04-17  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* xdvi.c (run_dvi_file): Avoid unaligned access on some 64-bit
	systems.  Mail from Nelson H. F. Beebe <beebe@math.utah.edu>
	[tlbuild] Mon, 16 Apr 2012 18:48:34.

2012-04-02  Peter Breitenlohner  <peb@mppmu.mpg.de>

	Imported xdvik-22.85.tar.gz
	from http://sourceforge.net/projects/xdvi/files/xdvik/.

	* mksedscript: Correctly handle strings with embedded blanks,
	e.g., '#define PACKAGE_STRING "xdvik 22.85"' and similar.

2012-03-17  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* mime.c, xdvi.1.in: Use /usr/local/etc/{mimetypes=>mime.types}.

2012-03-15  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* xdvi.1.in: Remove an unmatched #endif and formatting bugs.
	* Makefile.am: Fixed a typo (in comment).

2012-02-21  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* configure.ac: Drop XDVI_CC_CONCAT and XDVI_SYS_SUNOS_4, the
	results are not used.
	* m4/xdvi-cc-concat.m4, m4/xdvi-sys-sunos4.m4: Removed.

2011-12-06  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* Makefile.am: Replace ${prefix}/texmf => $(datarootdir)/texmf.

2011-07-30  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* font-open.[ch] (font_open): Constify `char **' parameter.
	* dvi-init.c (load_font): Cast font_open() argument to
	`const char **'.

2011-06-16  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* read-mapfile.c (read_map_file): Allow find_suffix() to return
	a const string.

2011-03-07  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* hypertex.c (launch_xdvi): `program_invocation_name' has been
	renamed into `kpse_invocation_name'.

2010-05-25  Karl Berry  <karl@tug.org>

	* m4/xdvi-check-iconv.m4 (--with-iconv): new option to
	completely disable checking for iconv.  Needed due to the local
	configuration of the machine used to build the sparc-linux
	binaries (namely, weyl.math.utah.edu).

2010-05-19  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* tests/run_tests.c, test_dl_list.c: Cast size_t to long for
	printing to avoid warnings on 64Bit systems.

2010-03-29  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* configure.ac: Remove AC_TYPE_SIZE_T, now part of KPSE_COMMON.

2009-12-22  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* tests/test_string_utils.c (test_my_stristr): Make the test
	more stringent (and closer to the original code).

2009-12-21  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* tests/test_string_utils.c (test_my_stristr): Make test result
	independent of compiler optimizations (constant folding).

2009-12-18  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* tests/test_string_utils.c (test_expand_filename):
	Do not prepend cwd to absolute filenames.

2009-11-17  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* configure.ac: Suppress some compiler warnings (too many).

2009-11-07  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* configure.ac, xdvi.h: Require recent kpathsea version.

2009-08-27  Peter Breitenlohner  <peb@mppmu.mpg.de>

	Avoid some maximal compiler warnings.
	* encodings.c (cjk2unicode): Unconstify string arg.
	* events.c (show_timers): Constify string arg.
	* gui/sfPath.c (SFLogin): Constify string members.
	(SFstrdup): Convert arg1 to result that can be assigned to const
	strings, constify remaining arg.
	(SFunreadableDir): Constify local var.
	* gui/sfPath.[ch] (SFchdir): Constify string arg.
	* hypertex.c (remote_URL_schemes): 'char *const'=>'const char*'.
	* main.c (compare_strings): 'const char**'=>'const char*const*'.
	* mime.c (mailcap_map): Constify string members.
	* pixmaps/toolbar.xpm, pixmaps/toolbar2.xpm: Constify pixmaps.


2009-08-26  Peter Breitenlohner  <peb@mppmu.mpg.de>

	Avoid some maximal compiler warnings.
	* dvi-init.c (get_tmp_dvi_name), gui/Tip.c (foo),
	gui/sfDir.c (foo), gui/sfDraw.c (foo), gui/sfPath.c (foo),
	gui/sfSelFile.c (foo), gui/xaw_menu.c (foo),
	gui/xlwradio.c (foo), gui/xm_colorsel.c (foo),
	gui/xm_filesel.c (foo), gui/xm_menu.c (foo),
	gui/xm_prefs.c (foo), gui/xm_prefs_appearance.c (foo),
	gui/xm_prefs_fonts.c (foo), gui/xm_prefs_helpers.c (foo),
	gui/xm_prefs_page.c (foo), gui/xm_prefs_scroll.c (foo),
	gui/xm_toolbar.c (foo), image-magick.c (clear_cache, foo),
	pagehist.c (page_history_clear),
	psdps.c (DPSErrorProcHandler, foo),
	psgs.c (gs_resume_prescan, foo),
	psnews.c (initNeWS, foo), util.c (do_abort):
	ANSI C function definitions, i.e., '()'=>'(void)'.
	* xserver-info.c (print_display_info, print_visual_info,
	print_screen_info, foo): ANSI C function definitions.

2009-08-22  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* configure.ac: Check for <kpathsea/version.h> and ...
	* xdvi.h: ... skip extern decl if found.

2009-06-29  Karl Berry  <karl@tug.org>

	* gui/help-window.c (default_help_general): avoid embedding
	date in binary.

2009-06-22  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* dvi-draw.c: do not #include "kpathsea/c-vararg.h" (after
	#include <stdarg.h>).
	* string-utils.h, xdvi.h, gui/message-window.c, 
	gui/statusline.c, tests/test_string_utils.c:
	#include <stdarg.h>, not "kpathsea/c-vararg.h".

	* xdvi.h: remove NeedFunctionPrototypes et al.

2009-06-03  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* events.c: avoid declaration after statement.

	from Vladimir Volovich <vvv@vsu.ru>.

2009-05-21  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* CHANGES: fixed version/relase-date such that mksedscript can
	extract them.

2009-04-28  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* xdvi.h: guard typedef of iconv_char_pptrT by '#if HAVE_ICONV_H'.

	* squeeze/squeeze.c: add '#include <string.h>'.

2009-04-18  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* xdvi.1.in (warnspecials): fixed duplicate entry, removing
	an #ifkpathsea without matching #endif.

2009-03-31  Peter Breitenlohner  <peb@mppmu.mpg.de>

	Imported ftp://tug.org/tex/xdvik-22.84.15.tar.gz and
	adapted to new TeX Live build system (Automake & Co.).
	Renamed original files XXX -> XXX-22.84.15.

	* squeeze.c (removed): File moved ...
	* squeeze/squeeze.c (new): ... to here. Don't include
	any system dependent kpathsea headers, such that squeeze/
	can be configured separately for the build system.

2009-03-21  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* CHANGES: fixed version/relase-date such that mksedscript can
	extract them.

2008-07-01  Peter Breitenlohner  <peb@mppmu.mpg.de>

	Imported ftp://tug.org/tex/xdvik-22.84.14.tar.gz and
	adapted to new TeX Live build system (autoconf-2.59+).

	* Makefile.in.orig, acconfig.h.orig, aclocal.m4.orig,
	configure.in.orig, depend.mk.orig, gui/Makefile.in.orig,
	gui/depend.mk.orig, kpathsea.ac.orig, tests/Makefile.in.orig,
	tests/depend.mk.orig, withenable.ac.orig (all new):
	Original files from xdvik-22.84.14.

	* Makefile.in, gui/Makefile.in, tests/Makefile.in:
	Added '@configure_input@' line and replace ac_include by
	kpse_include.

	* acinclude.m4 (new): Renamed from aclocal.m4.
	Renamed cache variables (must contain '_cv_').

	* acinclude.m4, configure.in, withenable.ac:
	Added lots of m4 quoting and third args of AC_DEFINE.

	* acconfig.h (removed): Use AC_DEFINE with three args to
	provide templates.

	* configure.in:
	Replaced AC_OUTPUT with arguments by KPSE_CONFIG_FILES,
	AC_CONFIG_FILES and AC_OUTPUT without arg.

	* kpathsea.ac (LIBKPATHSEACPPFLAGS): Set to '-I$(kpathsea_parent)'
	instead of '-I$(kpathsea_dir)' to avoid confusion between
	<c-auto.h> and <kpathsea/c-auto.h>.

	* depend.mk, gui/depend.mk, tests/depend.mk: Regenerated and
	cleaned by hand ('make depend' didn't quite do that). 

	* aclocal.m4, c-auto.in, configure: Regenerated.

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
