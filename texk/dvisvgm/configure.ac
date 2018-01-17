# $Id$
dnl Process this file with autoconf to produce a configure script
dnl for dvisvgm in TeX Live.
dnl
dnl   Copyright 2015-2018 Karl Berry <tex-live@tug.org>
dnl   Copyright 2009-2014 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
dnl  Adapted for TeX Live from original dvisvgm configure.ac
dnl  Copyright 2005-2018 Martin Gieseking
dnl
m4_include([version.ac])[] dnl define dvisvgm_version
AC_INIT([dvisvgm (TeX Live)], dvisvgm_version, [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([dvisvgm-src/src/dvisvgm.cpp])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIRS([../../m4])

AM_CONDITIONAL([TEXLIVE_BUILD], [test "x$enable_texlive_build" = xyes])

# Common code for all programs using libkpathsea.
# PROG_AR must become before KPSE_COMMON since that calls LT_INIT.
AM_PROG_AR
KPSE_COMMON([dvisvgm])

AC_CONFIG_HEADERS([config.h])

AC_DEFINE_UNQUOTED([TARGET_SYSTEM], ["$host"],
                   [The machine triplet of the target system.])

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
KPSE_POTRACE_FLAGS
KPSE_FREETYPE2_FLAGS
KPSE_ZLIB_FLAGS

KPSE_SAVE_FLAGS

# emacs-page
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
AS_IF([test "x$have_libgs" = "xno"],
	# Ghostscript not found, check for dlopen
	[AC_CHECK_LIB(dl, dlopen,,
		[AC_DEFINE(DISABLE_GS, 1, [Set to 1 if PostScript support should be disabled])]
		[AC_MSG_WARN(PostScript support disabled)])])
fi

if test "$HAVE_LIBGS" -eq 0; then
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
	if test "$gsversion" -lt 831; then
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
AC_CHECK_HEADERS([libintl.h stdlib.h string.h strings.h unistd.h])

# Check for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_STRUCT_TM

# Check for library functions.
AC_FUNC_CLOSEDIR_VOID
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_FUNC_STAT
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([ftime gettimeofday memset strcasecmp strtol uselocale])

AC_ARG_ENABLE([woff],
	[AS_HELP_STRING([--disable-woff], [Disable WOFF support @<:@no@:>@])],
	[],
	[enable_woff=yes])

AM_CONDITIONAL([ENABLE_WOFF], [test "x$enable_woff" = "xyes"])
if test "x$enable_woff" != "xyes" ; then
	AC_DEFINE([DISABLE_WOFF], 1, [Define if WOFF support is disabled])
fi

AC_DEFINE([HAVE_LIBKPATHSEA], 1, [Define to 1 if you have the `kpathsea' library (-lkpathsea).])
AC_DEFINE([HAVE_LIBFREETYPE], 1, [Define to 1 if you have the `freetype2' library (-lfreetype).])
AC_DEFINE([HAVE_LIBZ], 1, [Define to 1 if you have the `z' library (-lz).])
if false; then
  AC_DEFINE([KPSE_CXX_UNSAFE], 1, [Define to 1 if the kpathsea headers are not C++ safe.])
fi

# TL: not going to attempt code coverage.
AM_CONDITIONAL([CODE_COVERAGE_ENABLED], false)
CODE_COVERAGE_RULES=
AC_SUBST([CODE_COVERAGE_RULES])

AC_SUBST([DVISVGM_TREE], [dvisvgm-src])

AC_CONFIG_FILES([Makefile
	dvisvgm-src/Makefile
	dvisvgm-src/libs/Makefile
	dvisvgm-src/libs/brotli/Makefile
	dvisvgm-src/libs/clipper/Makefile
	dvisvgm-src/libs/ff-woff/Makefile
	dvisvgm-src/libs/variant/Makefile
	dvisvgm-src/libs/woff2/Makefile
	dvisvgm-src/libs/xxHash/Makefile
	dvisvgm-src/src/Makefile
	dvisvgm-src/src/version.hpp
])dnl TL: skipping tests/* and doc/*
AC_OUTPUT
