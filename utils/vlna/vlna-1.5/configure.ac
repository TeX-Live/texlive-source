dnl configure.ac -- Peter Breitenlohner <peb@mppmu.mpg.de> -- Public domain.

AC_PREREQ(2.60)
AC_INIT([vlna], [1.3], [tex-live@tug.org])
AC_CONFIG_SRCDIR([vlna.w])

AM_INIT_AUTOMAKE([foreign no-define])dnl using automake

# Checks for programs.
AC_PROG_CC
AC_PROG_MAKE_SET
AC_PROG_INSTALL

dnl Maintainer mode requires ctangle, cweave, and csplain
AC_CHECK_PROGS([CTANGLE], [ctangle], [none])
AC_CHECK_PROGS([CWEAVE], [cweave], [none])
AC_CHECK_PROGS([CSPLAIN], [csplain], [none])

if test x"$enable_maintainer_mode" = xyes; then
  case :$CTANGLE:$CWEAVE:$CSPLAIN: in
  *:none:*)
    AC_MSG_WARN([Maintainer mode disabled -- requires ctangle, cweave, and csplain.])
    enable_maintainer_mode=no
  ;;
  esac
fi

AM_MAINTAINER_MODE[]dnl maintainer mode only if requested

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
