dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2014 Taco Hoekwater <taco@metatex.org>
dnl   Copyright (C) 2014 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_include([version.ac])[] dnl define mpfr_version
AC_INIT([mpfr (TeX Live)], mpfr_version, [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([mpfr-]mpfr_version[/src/mpfr.h])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIRS([../../m4 m4])

KPSE_BASIC([mpfr])

AC_PROG_CC
AC_PROG_RANLIB
AC_PROG_LN_S

AC_CONFIG_HEADERS([config.h])

KPSE_CANONICAL_HOST

MPFR_CONFIGS

AM_CONDITIONAL([build], [test "x$enable_build" != xno])

KPSE_GMP_FLAGS

if test "x$enable_build" != xno || test -f config.force; then

AC_CHECK_HEADER([limits.h], [], [AC_MSG_ERROR([limits.h not found])])
AC_CHECK_HEADER([float.h], [],  [AC_MSG_ERROR([float.h not found])])
AC_CHECK_HEADER([string.h], [], [AC_MSG_ERROR([string.h not found])])

dnl Add some specific flags for ICC
AC_MSG_CHECKING([for ICC])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#if !defined __ICC
# error "ICC Not Found"
error
#endif
]])], [
 AC_MSG_RESULT([yes])
 CFLAGS="-fp_port -mp -wd1572 -wd265 -wd186 -wd239 $CFLAGS"
], [AC_MSG_RESULT([no])])

dnl This allows to take the first GMP library in the library paths,
dnl whether it is dynamic or static. This behavior is more sensible,
dnl in particular because it is the only way to link with a version
dnl only available in static form when another version is available
dnl in dynamic, and also for consistency, because the compiler will
dnl take the first gmp.h found in the include paths (so, we need to
dnl take a library that corresponds to this header file). This is a
dnl common problem with darwin.
AS_CASE([$host],
        [*-apple-darwin*], [MPFR_LD_SEARCH_PATHS_FIRST])

KPSE_ADD_FLAGS([gmp])

AC_CHECK_HEADER([gmp.h], [], [AC_MSG_ERROR([gmp.h not found])])

dnl Check minimal GMP version
dnl We only guarantee that with a *functional* and recent enough GMP version,
dnl MPFR will compile; we do not guarantee that GMP will compile.
dnl In particular fat builds are broken in GMP 4.3.2 and GMP 5.0.0
dnl (at least on 64-bit Core 2 under Linux),
dnl see http://gmplib.org/list-archives/gmp-bugs/2011-August/002345.html.
AC_MSG_CHECKING([for recent GMP])
AC_COMPILE_IFELSE([AC_LANG_SOURCE([[
#include "gmp.h"
#if (__GNU_MP_VERSION*100+__GNU_MP_VERSION_MINOR*10 < 410)
# error "min GMP version is 4.1.0"
error
#endif
]])], [AC_MSG_RESULT([yes])], [
 AC_MSG_RESULT([no])
 AC_MSG_ERROR([GMP 4.1.0 min required])
])

AC_CHECK_FUNC([__gmpz_init], [],
              [AC_MSG_ERROR([Sorry, cannot find/use libgmp])])

dnl Check for corresponding 'gmp.h' and libgmp.a
AC_MSG_CHECKING([if gmp.h version and libgmp version are the same])
AC_RUN_IFELSE([AC_LANG_PROGRAM([[
#include <stdio.h>
#include <string.h>
#include "gmp.h"
]], [[
  char buffer[100];
  sprintf (buffer, "%d.%d.%d", __GNU_MP_VERSION, __GNU_MP_VERSION_MINOR,
           __GNU_MP_VERSION_PATCHLEVEL);
  printf ("(%s/%s) ", buffer, gmp_version);
  if (strcmp (buffer, gmp_version) == 0)
    return 0;
  if (__GNU_MP_VERSION_PATCHLEVEL != 0)
    return 1;
  sprintf (buffer, "%d.%d", __GNU_MP_VERSION, __GNU_MP_VERSION_MINOR);
  return (strcmp (buffer, gmp_version) != 0) ? 1 : 0;
]])],
   [AC_MSG_RESULT([yes])
    MPFR_CHECK_PRINTF_SPEC],
   [AC_MSG_RESULT([no])
    AC_MSG_WARN([==========================================================])
    AC_MSG_WARN(['gmp.h' and 'libgmp' seem to have different versions or])
    AC_MSG_WARN([we cannot run a program linked with GMP (if you cannot])
    AC_MSG_WARN([see the version numbers above). A cause may be incompatible])
    AC_MSG_WARN([GMP versions See 'config.log' for details..])
    AC_MSG_WARN([==========================================================])
    ],
   [AC_MSG_RESULT([can not test])])


KPSE_RESTORE_FLAGS

echo timestamp >config.force
fi

AC_SUBST([MPFR_TREE], [mpfr-]mpfr_version)

AC_CONFIG_LINKS([mparam.h:mpfr-]mpfr_version[/src/mparam_h.in])

AC_CONFIG_FILES([Makefile include/Makefile])

AC_OUTPUT
