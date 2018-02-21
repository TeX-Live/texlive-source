#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([axohelp], [1.1], [jcc8@psu.edu])
AM_INIT_AUTOMAKE([no-define foreign])
AC_CHECK_LIB([m], [sqrt],,
  AC_MSG_ERROR([*** Please install libm on your system ***]))
AC_PROG_CC
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
