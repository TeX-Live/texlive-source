AC_PREREQ(2.54)
AC_INIT(teckit, 2.5.10, wstech@sil.org)
AM_CONFIG_HEADER([config.h])
AC_CONFIG_SRCDIR([source/UnicodeNames.cpp])
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE([1.11 subdir-objects])

AM_MAINTAINER_MODE
AM_SILENT_RULES([yes])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_INSTALL
AC_PROG_LIBTOOL

case "${target_os}" in
mingw32*)
LT_PROG_RC
;;
esac

AC_ARG_WITH(old-lib-names,
[  --with-old-lib-names    Use old library names (for Windows only)],
[case "${withval}" in
  yes) oldnames=true ;;
  no)  oldnames=false ;;
  *) AC_MSG_ERROR(bad value ${withval} for --with-old-lib-names) ;;
esac],[oldnames=false])
AM_CONDITIONAL(OLD_LIB_NAMES, test x$oldnames = xtrue)

# Checks for libraries.
AC_ARG_ENABLE(tetex-build,
  AC_HELP_STRING([--enable-tetex-build],
    [build within teTeX/TeXLive. (default=no)]))
if test x$enable_tetex_build = xyes; then
  AC_MSG_NOTICE([Building within teTeX/TeXLive])
  sinclude(../zlib/withenable.ac)
  sinclude(../zlib/zlib.ac)
  ZLIBDIR=../zlib
else
  AC_MSG_NOTICE([Not doing a teTeX/TeXLive-style build])
  sinclude(zlib-1.2.11/withenable.ac)
  sinclude(zlib-1.2.11/zlib.ac)
  ZLIBDIR=zlib-1.2.11
fi
AC_SUBST(ZLIBDIR)

AM_CONDITIONAL(SYSTEM_ZLIB, test x$using_system_zlib = xyes)

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h])


AC_C_BIGENDIAN

# Enable debug
AC_ARG_ENABLE(debug,
  AC_HELP_STRING([--enable-debug],
    [build for debugging. (default=no)]))
if test "$enable_debug" = yes; then
  CFLAGS="$CFLAGS -O0"
  CXXFLAGS="$CXXFLAGS -O0"
  if test "$ac_cv_prog_cc_g" = yes; then
    CFLAGS="$CFLAGS -g"
  fi
  if test "$ac_cv_prog_cxx_g" = yes; then
    CXXFLAGS="$CXXFLAGS -g"
  fi
  CFLAGS="$CFLAGS -Wno-unknown-pragmas -Wall -Wextra -Werror "
  CXXFLAGS="$CXXFLAGS -Wno-unknown-pragmas -Wall -Wextra -Werror "
#  AC_DEFINE(DEBUG)
else
    CFLAGS="$CFLAGS -DNDEBUG"
    CXXFLAGS="$CXXFLAGS -DNDEBUG"
fi

# Enable final
AC_ARG_ENABLE(final,
  AC_HELP_STRING([--enable-final],
    [build for release. (default=no)]))
if test "$enable_final" = yes; then
  CFLAGS="$CFLAGS -O3 -DNDEBUG"
  CXXFLAGS="$CXXFLAGS -O3 -DNDEBUG"
fi

# Enable profile
AC_ARG_ENABLE(profile,
  AC_HELP_STRING([--enable-profile],
    [build for profiling. (default=no)]))
if test "$enable_profile" = yes; then
  CFLAGS="$CFLAGS -pg "
  CXXFLAGS="$CXXFLAGS -pg "
fi
AC_ARG_ENABLE(profilefn,
  AC_HELP_STRING([--enable-profilefn],
    [build with function instrumentation. (default=no)]))
if test "$enable_profilefn" = yes; then
  CFLAGS="$CFLAGS -g -finstrument-functions "
  CXXFLAGS="$CXXFLAGS -g -finstrument-functions "
  LIBS="$LIBS -g -finstrument-functions -lfnccheck "
fi

# We only want expat for sfconv, so remember the libs setting without it
# (this seems really hacky, but I don't know the proper way....!)
noexpat_CFLAGS="$CFLAGS"
noexpat_LIBS="$LIBS"
AC_CHECK_LIB(expat, XML_ExpatVersion)
AM_CONDITIONAL(SYSTEM_EXPAT, test x$ac_cv_lib_expat_XML_ExpatVersion = xyes)
expat_CFLAGS=""
expat_LIBS="$LIBS"
CFLAGS="$noexpat_CFLAGS"
LIBS="$noexpat_LIBS"
AC_SUBST(expat_CFLAGS)
AC_SUBST(expat_LIBS)

AC_CONFIG_FILES([ Makefile \
        teckit.pc:installed-top.pc.in:teckit.pc.in \
        teckit-uninstalled.pc:uninstalled-top.pc.in:teckit.pc.in \
        lib/Makefile bin/Makefile docs/Makefile test/Makefile])

AC_OUTPUT
