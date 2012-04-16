#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([mtx], [0.60d], [dpl@sun.ac.za])
AC_CONFIG_SRCDIR([analyze.c])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE(mtx,0.60d)

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_PROG_RANLIB

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([memory.h stddef.h stdlib.h string.h strings.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_FUNC_STRTOD
AC_CHECK_FUNCS([memchr memmove memset strchr strrchr strtol])

AC_CONFIG_FILES([Makefile
                 libp2c/Makefile])
AC_OUTPUT
