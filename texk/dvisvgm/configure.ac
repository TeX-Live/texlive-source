# $Id$
dnl Process this file with autoconf to produce a configure script
dnl for dvisvgm in TeX Live.
dnl
dnl   Copyright 2015-2020 Karl Berry <tex-live@tug.org>
dnl   Copyright 2009-2014 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
dnl  Adapted for TeX Live from original dvisvgm configure.ac
dnl  Copyright 2005-2020 Martin Gieseking
dnl
AC_PREREQ([2.65])
m4_include([version.ac])[] dnl define dvisvgm_version
AC_INIT([dvisvgm (TeX Live)], dvisvgm_version, [tex-k@tug.org])
DATE="March 2020"
AC_CONFIG_SRCDIR([dvisvgm-src/src/dvisvgm.cpp])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIRS([../../m4])dnl not just _DIR
AC_CONFIG_AUX_DIR([../../build-aux])

AM_CONDITIONAL([TEXLIVE_BUILD], [test "x$enable_texlive_build" = xyes])

# Common code for all programs using libkpathsea.
# PROG_AR must be run before KPSE_COMMON, since the latter calls LT_INIT.
AM_PROG_AR
KPSE_COMMON([dvisvgm])

AH_TEMPLATE([TARGET_SYSTEM], [The machine triplet of the target system])
AC_DEFINE_UNQUOTED([TARGET_SYSTEM], ["$target"])

AC_PROG_CC
AC_PROG_CC_C99
AC_PROG_CXX
AX_CXX_COMPILE_STDCXX([11])
LT_INIT
KPSE_CXX_HACK

AC_PROG_RANLIB
AC_LANG(C)

AC_CHECK_HEADERS([sys/time.h sys/timeb.h xlocale.h])
AC_HEADER_TIOCGWINSZ

KPSE_COND_WIN32
KPSE_KPATHSEA_FLAGS
KPSE_FREETYPE2_FLAGS
KPSE_ZLIB_FLAGS

KPSE_SAVE_FLAGS

# emacs-page Ghostscript complexities.
LIBGS_INCLUDES=
LIBGS_LIBS=
if test "x$enable_native_texlive_build" = xyes; then
  # TL: always dlload libgs, must avoid shared lib ref in binary.
  HAVE_LIBGS=0
else
have_libgs=yes
AC_CHECK_HEADER([ghostscript/iapi.h],
	[AC_CHECK_LIB(gs, gsapi_revision,, [have_libgs=no])],
	[have_libgs=no])
if test "x$have_libgs" = xyes; then
  HAVE_LIBGS=1
fi
AS_IF([test "x$have_libgs" = "xno"],
	# Ghostscript not found, check for dlopen
	[AC_CHECK_LIB(dl, dlopen,,
		[AC_DEFINE(DISABLE_GS, 1, [Set to 1 if PostScript support should be disabled])]
		[AC_MSG_WARN(PostScript support disabled)])])
fi

if test -z "$HAVE_LIBGS" || test "$HAVE_LIBGS" -eq 0; then
  AC_MSG_NOTICE([not linking to libgs, trying to arrange for dynamic loading])
  # Windows (native or MinGW32) has neither <dlfcn.h> nor dlopen().
  if test "x$kpse_cv_have_win32" = xno; then
    # FreeBSD neither has nor requires libdl.
    AC_SEARCH_LIBS([dlopen], [dl])
    AS_CASE([$ac_cv_search_dlopen],
            [no | "none required"], [],
                [LIBGS_LIBS=$ac_cv_search_dlopen])
    AC_CHECK_HEADER([dlfcn.h])
    AC_CHECK_FUNC([dlopen])
    if test "x$ac_cv_header_dlfcn_h:$ac_cv_func_dlopen" != xyes:yes; then
      AC_DEFINE(DISABLE_GS, 1,
                [Set to 1 if PostScript support is to be disabled])
      LIBGS_LIBS=
    fi
  fi
else
	# query Ghostscript version
	AC_MSG_CHECKING([Ghostscript version])
	AC_RUN_IFELSE([AC_LANG_SOURCE([#include <stdio.h>
		#include <ghostscript/iapi.h>
		int main () {
			gsapi_revision_t r;
			if (gsapi_revision(&r, sizeof(gsapi_revision_t)) == 0) {
				FILE *f;
				f = fopen("gsversion", "w");
				if (!f) exit(1);
				fprintf(f, "%ld\n", r.revision);
				fclose(f);
				exit(0);
			}
		}])],
		[gsversion=`cat gsversion`], [gsversion=], [gsversion=unknown])
	AC_MSG_RESULT("$gsversion")
	rm -f gsversion
	if test -z "$gsversion" || test "$gsversion" -lt 831; then
		# current Ghostscript API was introduced in version 8.31
		# older versions are not supported
		AC_DEFINE(DISABLE_GS, 1, [Set to 1 if PostScript support should be disabled])
		AC_MSG_WARN([Ghostscript version < 8.31 found; PostScript support disabled])
	else
		LIBGS_LIBS=-lgs
	fi
fi
AC_SUBST([LIBGS_INCLUDES])
AC_SUBST([LIBGS_LIBS])

if test "x$enable_build" != xno || test -f config.force; then

# Checks for more libraries.
KPSE_ADD_FLAGS([zlib])
AC_CHECK_FUNC([gzopen], [],
              [AC_MSG_ERROR([cannot find/use zlib])])

KPSE_ADD_FLAGS([freetype2])
AC_CHECK_FUNC([FT_Init_FreeType], [],
              [AC_MSG_ERROR([cannot find/use libfreetype])])

KPSE_ADD_FLAGS([kpathsea])
AC_CHECK_FUNC([kpse_set_program_name], [],
              [AC_MSG_ERROR([cannot find/use libkpathsea])])

# Check if the kpathsea headers are C++ safe.
AC_LANG_PUSH([C++])
AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <kpathsea/kpathsea.h>]],
                                [[printf("%s\n", concat("one", "two"))]])],
               [],
               [AC_MSG_ERROR([Sorry, your kpathsea headers are too old])
                AC_DEFINE([KPSE_CXX_UNSAFE], 1,
                          [Define to 1 if the kpathsea headers are not C++ safe.])])
AC_LANG_POP([C++])

echo timestamp >config.force
fi

KPSE_RESTORE_FLAGS

# emacs-page
# Check for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_CHECK_HEADERS_ONCE([libintl.h stdlib.h string.h strings.h unistd.h])

# Check for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_SIZE_T
AC_STRUCT_TM

# Check for library functions.
AC_FUNC_STAT
AC_CHECK_FUNCS_ONCE([ftime gettimeofday sigaction umask uselocale])

AC_ARG_ENABLE([woff],
	[AS_HELP_STRING([--disable-woff], [Disable WOFF support @<:@default=no@:>@])],
	[],
	[enable_woff=yes])
AM_CONDITIONAL([ENABLE_WOFF], [test "x$enable_woff" = "xyes"])

AC_DEFINE([HAVE_LIBKPATHSEA], 1, [Define to 1 if you have the `kpathsea' library (-lkpathsea).])
AC_DEFINE([HAVE_LIBFREETYPE], 1, [Define to 1 if you have the `freetype2' library (-lfreetype).])
AC_DEFINE([HAVE_LIBZ], 1, [Define to 1 if you have the `z' library (-lz).])
if false; then
  AC_DEFINE([KPSE_CXX_UNSAFE], 1, [Define to 1 if the kpathsea headers are not C++ safe.])
fi

# TL: skip ttfautohint?

# Add option to enable linking of bundled libraries (brotli, potrace, woff2, xxhash).
AC_ARG_ENABLE([bundled-libs],
	[AS_HELP_STRING([--enable-bundled-libs], [use bundled libraries instead of the system ones @<:@default=no@:>@])],
	[use_bundled_libs=yes])
AM_CONDITIONAL([USE_BUNDLED_LIBS], [test "x$use_bundled_libs" = "xyes" dnl
                               || test "x$enable_native_texlive_build" = xyes])
# that is, force using bundled libs for native TL build.

# TL: use bundled md5 too
AM_CONDITIONAL([USE_BUNDLED_MD5], true)

# TL: these conditionals for system libraries are in the Makefile.am's.
AM_CONDITIONAL(HAVE_POTRACE, false)
AM_CONDITIONAL(HAVE_BROTLI, false)
AM_CONDITIONAL(HAVE_WOFF2, false)
AM_CONDITIONAL(HAVE_XXHASH, false)

# TL: not going to build_manpage (which is the default).
# TL: not going to attempt code coverage; need to subst it away.
AM_CONDITIONAL([CODE_COVERAGE_ENABLED], false)
CODE_COVERAGE_RULES=
AC_SUBST([CODE_COVERAGE_RULES])

# TL: the original configure calls pkg_check_modules,
# which calls ac_arg_var on the _cflags and _libs for various libraries,
# which calls ac_subst. we'll just ac_subst them to get initial (empty)
# definitions, so we can retain the += used by the original lib/defs.am.
# (init brotli)
AC_SUBST(BROTLI_CFLAGS)
AC_SUBST(BROTLI_LIBS)
# (init woff2)
AC_SUBST(WOFF2_CFLAGS)
AC_SUBST(WOFF2_LIBS)

AC_SUBST([DVISVGM_TREE], [dvisvgm-src])

AC_SUBST([dvisvgm_srcdir], ['$(top_srcdir)/dvisvgm-src'])
AC_SUBST(DATE)
AC_SUBST(AM_CPPFLAGS)
AC_SUBST(AM_LDFLAGS)
AC_CONFIG_FILES([Makefile
	dvisvgm-src/Makefile
	dvisvgm-src/libs/Makefile
	dvisvgm-src/libs/brotli/Makefile
	dvisvgm-src/libs/clipper/Makefile
	dvisvgm-src/libs/ff-woff/Makefile
	dvisvgm-src/libs/md5/Makefile
	dvisvgm-src/libs/potrace/Makefile
	dvisvgm-src/libs/variant/Makefile
	dvisvgm-src/libs/woff2/Makefile
	dvisvgm-src/libs/xxHash/Makefile
	dvisvgm-src/src/Makefile
	dvisvgm-src/src/version.hpp
	dvisvgm-src/src/optimizer/Makefile
])dnl TL: skipping tests/* and doc/*
AC_OUTPUT
