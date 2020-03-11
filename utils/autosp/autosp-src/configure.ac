AC_PREREQ(2.63)
AC_INIT([autosp],[2020-03-11],[rdt@cs.queensu.ca])
AC_CONFIG_AUX_DIR([.])
AM_INIT_AUTOMAKE
AC_CONFIG_HEADERS([config.h])

m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h stdio.h time.h getopt.h ctype.h limits.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_SIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([strstr])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
