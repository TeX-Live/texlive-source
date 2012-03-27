#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([pmx], [2.6.17], [dsimons@roadrunner.com])
AC_CONFIG_SRCDIR([pmx2617.c])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE(pmx, 2.6.17)

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([m], [sqrt])

# Checks for header files.
AC_CHECK_HEADERS([fenv.h])
AC_CHECK_HEADERS([stdlib.h])
AC_CHECK_HEADERS([string.h])


# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([sqrt])
AC_CHECK_FUNCS([atexit])
AC_CHECK_FUNCS([floor])
AC_CHECK_FUNCS([memset])
AC_CHECK_FUNCS([pow])
AC_FUNC_MALLOC
AC_FUNC_REALLOC



AC_CONFIG_FILES([Makefile])
AC_OUTPUT
