dnl See ./README for our basic strategy here.
dnl
dnl   Copyright (C) 2011, 2012 Karl Berry <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
AC_INIT([Biber], [TeX Live], [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([TeXLive/TLConfig.pm])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

AM_INIT_AUTOMAKE([foreign])
AM_MAINTAINER_MODE

dnl Checks for programs.
AC_PROG_INSTALL
AC_PROG_MAKE_SET

BIBER_BINARY=

if test "x$enable_build" != xno || test -f config.force; then

dnl Determine the TeX Live platform name.
KPSE_TL_PLATFORM

dnl Check if we have an executable for this platform.
AC_MSG_CHECKING([for a biber binary])
biber_bin="bin/$TL_PLATFORM/biber"
if test -n "$ac_exeext" && test -r "$srcdir/$biber_bin$ac_exeext"; then
  BIBER_BINARY="$biber_bin$ac_exeext"
elif test -r "$srcdir/$biber_bin"; then
  if echo "$TL_PLATFORM" | grep freebsd >/dev/null 2>&1; then

    # on freebsd, we don't want to install the binary if the current
    # version is too old.  The standard way to do this is to look in
    # sys/param.h.  Use standard FreeBSD program locations since that's
    # what they do.
    : ${AWK=/usr/bin/awk}
    : ${SYSCTL=/sbin/sysctl}
    if test -n "$OSVERSION"; then
      if test -r /usr/include/sys/param.h; then
        OSVERSION=`$AWK '/^\#define[[:blank:]]__FreeBSD_version/ {print $3}'\
                   </usr/include/sys/param.h`
      elif test -r /usr/src/sys/sys/param.h; then
        OSVERSION=`$AWK '/^\#define[[:blank:]]__FreeBSD_version/ {print $3}'\
                   </usr/src/sys/sys/param.h`      
      else
        OSVERSION=`${SYSCTL} -n kern.osreldate`
      fi
    fi
    # check if freebsd os version is new enough.
    if test "$OSVERSION" -ge 701000; then
      BIBER_BINARY="$biber_bin"
    fi
  else
    # not freebsd, just take it.
    BIBER_BINARY="$biber_bin"
  fi
fi
AC_MSG_RESULT([$BIBER_BINARY])

echo timestamp >config.force
fi

AC_SUBST([BIBER_BINARY])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
