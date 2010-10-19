#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

m4_define([musixflx_version], [0.83.2])[]
AC_PREREQ(2.59)
AC_INIT([musixflx], musixflx_version, [ross.mitchell@csiro.au])
AM_INIT_AUTOMAKE

AC_CONFIG_SRCDIR([musixflx.c])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_CHECK_FUNCS([strchr strrchr])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
