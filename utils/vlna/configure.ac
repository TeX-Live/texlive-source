dnl configure.ac -- Peter Breitenlohner <peb@mppmu.mpg.de> -- Public domain.

AC_PREREQ(2.62)
AC_INIT([vlna], [1.3], [tex-live@tug.org])
AC_CONFIG_SRCDIR([vlna.w])

AM_INIT_AUTOMAKE([foreign no-define])dnl using automake

AM_MAINTAINER_MODE[]dnl maintainer mode only if requested
AC_DISABLE_OPTION_CHECKING[]dnl disable warning for unrecognized inherited options

# Checks for programs.
AC_PROG_CC
AC_PROG_MAKE_SET
AC_PROG_INSTALL

dnl Maintainer mode requires ctangle, cweave, and csplain
AC_CHECK_PROGS([CTANGLE], [ctangle], [none])
AC_CHECK_PROGS([CWEAVE], [cweave], [none])
AC_CHECK_PROGS([CSPLAIN], [csplain], [none])

if test x"$USE_MAINTAINER_MODE" = xyes; then
  case :$CTANGLE:$CWEAVE:$CSPLAIN: in
  *:none:*)
    AC_MSG_ERROR([Maintainer mode requires ctangle, cweave, and csplain in your PATH.])
  ;;
  esac
fi

AM_CONDITIONAL([TETEX_BUILD], [test x"$enable_tetex_build" = xyes])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
