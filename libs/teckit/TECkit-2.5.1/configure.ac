AC_PREREQ(2.54)
AC_INIT(teckit, 2.5.1, jonathan_kew@sil.org)
AM_CONFIG_HEADER([config.h])
AC_CONFIG_SRCDIR([source/UnicodeNames.cpp])
AC_CANONICAL_SYSTEM

AM_INIT_AUTOMAKE

AM_MAINTAINER_MODE

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_INSTALL
AC_PROG_LIBTOOL

if test "${target_os}" = "mingw32"; then
AC_PROG_RC
fi

AC_ARG_WITH(old-lib-names,
[  --with-old-lib-names    Use old library names (for Windows only)],
[case "${withval}" in
  yes) oldnames=true ;;
  no)  oldnames=false ;;
  *) AC_MSG_ERROR(bad value ${withval} for --with-old-lib-names) ;;
esac],[oldnames=false])
AM_CONDITIONAL(OLD_LIB_NAMES, test x$oldnames = xtrue)

# Checks for libraries.
AC_CHECK_LIB(z, compress)
AM_CONDITIONAL(SYSTEM_ZLIB, test x$ac_cv_lib_z_compress = xyes)

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h])


AC_C_BIGENDIAN

# Enable debug
if test "$enable_debug" = yes; then
  CFLAGS="$CFLAGS -O0"
  CXXFLAGS="$CXXFLAGS -O0"
  if test "$ac_cv_prog_cc_g" = yes; then
    CFLAGS="$CFLAGS -g"
  fi
  if test "$ac_cv_prog_cxx_g" = yes; then
    CXXFLAGS="$CXXFLAGS -g"
  fi
  CFLAGS="$CFLAGS -Wno-unknown-pragmas -Wparentheses -Werror "
  CXXFLAGS="$CXXFLAGS -Wno-unknown-pragmas -Wparentheses -Werror "
#  AC_DEFINE(DEBUG)
else
    CFLAGS="$CFLAGS -DNDEBUG"
    CXXFLAGS="$CXXFLAGS -DNDEBUG"
fi

# Enable final
if test "$enable_final" = yes; then
  CFLAGS="-O3 -DNDEBUG"
  CXXFLAGS="-O3 -DNDEBUG"
fi

# Enable profile
if test "$enable_profile" = yes; then
  CFLAGS="$CFLAGS -pg "
  CXXFLAGS="$CXXFLAGS -pg "
fi
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
expat_CFLAGS="$CFLAGS"
expat_LIBS="$LIBS"
CFLAGS="$noexpat_CFLAGS"
LIBS="$noexpat_LIBS"
AC_SUBST(expat_CFLAGS)
AC_SUBST(expat_LIBS)

AC_CONFIG_FILES([ Makefile lib/Makefile bin/Makefile docs/Makefile test/Makefile])

AC_OUTPUT
