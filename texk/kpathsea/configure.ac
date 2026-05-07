dnl $Id$
dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright 2015-2026 Karl Berry <tex-live@tug.org>
dnl   Copyright 2009-2015 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_include([version.ac])[] dnl define kpse_version
AC_INIT([Kpathsea], kpse_version, [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([pathsearch.c])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

AC_SUBST([KPSEVERSION], [kpse_version])
KPSE_LT_VERSION([kpse])

m4_include([../../version.ac])[] dnl define tex_live_version
AC_SUBST([WEB2CVERSION], [" (TeX Live tex_live_version)"])

dnl Common code for all programs using libkpathsea.
KPSE_COMMON([kpathsea])
dnl Include additional code for kpathsea.
KPSE_MKTEX_PREPARE
m4_sinclude([ac/mktex.ac])

dnl Generate MAKE_TEX_*_BY_DEFAULT and MAKE_OMEGA_*_BY_DEFAULT defines.
m4_foreach([Kpse_Mktex], [kpse_mktex_opts],
           [m4_ifset([Kpse_Mktex],
                     [KPSE_MKTEX_DEFINE(Kpse_Mktex)])])[]dnl

# So we can use reasonable awk in the Makefile.
AC_PROG_AWK

AC_CHECK_SIZEOF([long])

dnl Replacement functions that may be required on ancient broken system.
AC_CHECK_DECLS([putenv])
AC_REPLACE_FUNCS([putenv])

dnl We used to check if getcwd forked, when TeX was among the biggest
dnl programs likely to be run (1980s), and forking could easily cause a
dnl gratuitous out-of-memory error. Nowadays, this is not the case.
dnl Also, hopefully every getcwd() has become smart enough not to fork.
dnl Also, the fallback definition of getcwd provided in xgetcwd.c fails
dnl under NFS. All in all, seems better to just always use the system
dnl getcwd, for better or worse. Check removed in May 2026.
dnl 
dnl if test "x$ac_cv_func_getcwd" = xyes; then
dnl   # We only need to run this if we have getcwd.
dnl   # Don't declare getcwd(), since C23 doesn't allow empty parens.
dnl   # Instead, we just have to hope that it's declared in unistd.h
dnl   # where it should be; stdlib.h is for exit.
dnl   AC_CACHE_CHECK([whether getcwd uses fork or vfork],
dnl                [kb_cv_func_getcwd_forks],
dnl                [AC_RUN_IFELSE([AC_LANG_PROGRAM([[#include <unistd.h>
dnl                                                  #include <stdlib.h>
dnl                                                  int fork()  { exit(1); }
dnl                                                  int vfork() { exit(1); }
dnl                                                  char path[100];]],
dnl                                                [[getcwd(path,100);]])],
dnl                               [kb_cv_func_getcwd_forks=no],
dnl                               [kb_cv_func_getcwd_forks=yes],
dnl                               [kb_cv_func_getcwd_forks=no])])
dnl   if test "x$kb_cv_func_getcwd_forks" = xyes; then
dnl     AC_DEFINE([GETCWD_FORKS], 1,
dnl             [Define to 1 if getcwd is implemented using fork or vfork.
dnl              Email tex-k@tug.org with your system info and config.log if
dnl              you have to add this by hand because configure failed to
dnl              detect it.])
dnl   fi # forks=yes
dnl fi   # have_getcwd

KPSE_COND_MINGW32

AC_SUBST([LT_OBJDIR], ["$lt_cv_objdir"])

# Checking snprintf and vsnprintf
AC_ARG_WITH([snprintf-wrapper],
            AS_HELP_STRING([--with-snprintf-wrapper],
                           [use (v)snprintf wrapper functions @<:@automatic
                            for native compilation or Windows@:>@]),
            [kpse_cv_wrap_snprintf=$withval],
            [test "x$kpse_cv_have_win32" != xno && kpse_cv_wrap_snprintf=yes])[]dnl
if test "x$kpse_cv_have_win32" = xno; then
  snfunc=snprintf vsnfunc=vsnprintf
else
  snfunc=_snprintf vsnfunc=_vsnprintf
fi
AC_CACHE_CHECK([for $snfunc and $vsnfunc],
               [kpse_cv_have_snfuncs],
               [AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <stdarg.h>
                                                  #include <stdio.h>
                                                  char buf[16];
                                                  va_list ap;]],
                                                [[$snfunc (buf, 16, "%s", "abc");
                                                  $vsnfunc (buf, 16, "%s", ap);]])],
                               [kpse_cv_have_snfuncs=yes],
                               [kpse_cv_have_snfuncs=no])])
if test "x$kpse_cv_have_snfuncs" != xyes; then
  AC_MSG_ERROR([Sorry, you need $snfunc and $vsnfunc.])
fi
AC_RUN_IFELSE([AC_LANG_PROGRAM([[#include <stdarg.h>
                                 #include <stdio.h>
                                 char buf[4] = "abc";]],
                               [[if ($snfunc (buf, 1, "%s", "x") == 0 || buf[1] != 'b')
                                   return 1;]])],
              [],
              [AC_MSG_ERROR([Sorry, your $snfunc is badly broken.])],
              [:])
AC_CACHE_CHECK([if we need (v)snprintf wrapper functions],
               [kpse_cv_wrap_snprintf],
               [AC_RUN_IFELSE([AC_LANG_PROGRAM([[#include <stdarg.h>
                                                 #include <stdio.h>
                                                 char buf[4] = "abc";]],
                                               [[$snfunc (buf, 3, "xyz");
                                                 if (buf[2] != 0) return 1;
                                                 $snfunc (buf, 2, "xyz");
                                                 if (buf[1] != 0) return 1;]])],
                              [kpse_cv_wrap_snprintf=no],
                              [kpse_cv_wrap_snprintf=yes],
                              [kpse_cv_wrap_snprintf=no])])
if test "x$kpse_cv_wrap_snprintf" = xyes; then
  AC_DEFINE([WRAP_SNPRINTF], 1,
            [Define to 1 if we need (v)snprintf wrapper functions.])
fi               

AM_CONDITIONAL([have_EXEEXT], [test -n "$EXEEXT"])

dnl Write output here, instead of putting a zillion -D's on the command line.
AC_CONFIG_HEADERS([c-auto.h:c-auto.in],
 [sed -e 's/^#define PACKAGE/#define KPSE_PACKAGE/' \
      -e 's/^#define VERSION/#define KPSE_VERSION/' c-auto.h >c-auto.tmp && mv -f c-auto.tmp c-auto.h])

AH_TOP([/* c-auto.h: defines for kpathsea, as determined by configure.

   Copyright 1994-97, 2008-2015 Karl Berry.
   Copyright 1997-99, 2002, 2005 Olaf Weber.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this library; if not, see <http://www.gnu.org/licenses/>.  */

/* Guard against double inclusion. */
#ifndef KPATHSEA_C_AUTO_H
#define KPATHSEA_C_AUTO_H

/* kpathsea: the version string. */
#define KPSEVERSION "kpathsea version] kpse_version["])
AH_BOTTOM([#endif /* !KPATHSEA_C_AUTO_H */])

AC_CONFIG_FILES([Makefile doc/Makefile man/Makefile win32/Makefile kpathsea.pc])

AC_OUTPUT
