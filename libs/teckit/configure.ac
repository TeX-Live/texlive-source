AC_PREREQ(2.54)
AC_INIT(teckit, 2.1.5, jonathan_kew@sil.org)
AM_CONFIG_HEADER([config.h])
AC_CONFIG_SRCDIR([source/UnicodeNames.cpp])
AM_INIT_AUTOMAKE

# Optional features
AM_MAINTAINER_MODE

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LIBTOOL

# Checks for libraries.
## AC_CHECK_LIB(z, compress)
sinclude(../zlib/withenable.ac)
sinclude(../zlib/zlib.ac)

## omit this for TL - we don't need sfconv
## AC_CHECK_LIB(expat, XML_ExpatVersion)
## AM_CONDITIONAL(SYSTEM_EXPAT, test x$ac_cv_lib_expat_XML_ExpatVersion = xyes)

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([limits.h malloc.h memory.h stddef.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
#AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_FUNC_REALLOC
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

AC_CONFIG_FILES([ Makefile lib/Makefile bin/Makefile docs/Makefile test/Makefile])

AC_OUTPUT
