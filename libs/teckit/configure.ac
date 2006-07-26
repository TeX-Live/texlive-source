AC_PREREQ(2.54)
AC_INIT(teckit, 2.1.3, jonathan_kew@sil.org)
AM_CONFIG_HEADER([config.h])
AC_CONFIG_SRCDIR([source/UnicodeNames.cpp])
AM_INIT_AUTOMAKE

# Optional features
#AC_ARG_ENABLE(grcompiler,
#    AC_HELP_STRING([--enable-grcompiler],
#        [build the Graphite font compiler. (default=no)]),
#    [package_grcompiler_enabled=yes], [package_grcompiler_enabled=no])
AM_MAINTAINER_MODE


# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LIBTOOL

# Checks for libraries.
AC_CHECK_LIB(z, compress)
AC_CHECK_LIB(expat, XML_ExpatVersion)

AM_CONDITIONAL(SYSTEM_EXPAT, test x$ac_cv_lib_expat_XML_ExpatVersion = xyes)

# Checks for header files.
#AC_HEADER_STDC
#AC_CHECK_HEADERS([limits.h malloc.h memory.h stddef.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
#AC_TYPE_SIZE_T
#AC_STRUCT_TM
#AC_CHECK_SIZEOF(wchar_t)
#AC_SUBST(SIZEOF_WCHAR_T)

# Checks for library functions.
#AC_FUNC_ERROR_AT_LINE
#AC_FUNC_MALLOC
#AC_FUNC_MEMCMP
#AC_FUNC_REALLOC
#AC_FUNC_VPRINTF
#AC_CHECK_FUNCS([isascii memmove memset strcasecmp strchr strrchr strtoul])
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


AC_CONFIG_FILES([ Makefile lib/Makefile bin/Makefile docs/Makefile test/Makefile])

AC_OUTPUT
