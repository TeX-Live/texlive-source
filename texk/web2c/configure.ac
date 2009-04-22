dnl Process this file with Autoconf to produce a configure script for Web2c.
dnl
dnl   Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
dnl --------------------------------------------------------
dnl
dnl Some things are no longer configurable:
dnl - SMALL{TeX,MF,BibTeX}: too painful to maintain the separate patch
dnl   files, and, for TeX, texmfmem.h would have to be fixed to support
dnl   more than 256 fonts.
dnl - NONASCII: necessary for the character translation feature.
dnl - REGFIX: modern compilers mostly ignore register declarations, anyway.
dnl The code for these things remains (where applicable), so you can
dnl get these features if you are willing to hack the sources. If not,
dnl it'll take a good argument to convince me to invest the time to make
dnl them configurable.
dnl
m4_define([web2_version], [7.5.7])
AC_INIT([Web2C], web2_version(), [tex-k@tug.org])
AC_PREREQ([2.63])
dnl 
dnl We don't use (for example) tex/tex.web because people who only want
dnl to build part of the distribution may not have any given program.
dnl Even cpascal.h isn't guaranteed, but then nothing is, really ...
AC_CONFIG_SRCDIR([cpascal.h])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

WEB2CVERSION=web2_version()
AC_SUBST([WEB2CVERSION])
AC_DEFINE_UNQUOTED([WEB2CVERSION], [" (Web2C $WEB2CVERSION)"])

dnl Common code for all programs using libkpathsea.
KPSE_COMMON([web2c programs])

AC_PROG_CXX
KPSE_CXX_HACK

dnl Include additional code for web2c.
KPSE_WEB2C_PREPARE
m4_include([ac/web2c.ac])

KPSE_ASM_UNDERSCORE

AS_CASE([$with_editor], [''|yes|no], [with_editor='vi +%d %s'])
AC_DEFINE_UNQUOTED([EDITOR], ["$with_editor"],
                   [web2c: Default editor for interactive `e' option.])

AS_IF([test "x$enable_auto_core" = xyes],
      [AC_DEFINE([FUNNY_CORE_DUMP], 1,
                 [web2c: Define to enable HackyInputFileNameForCoreDump.tex.])])

AS_IF([test "x$enable_dump_share" = xno],
      [AC_DEFINE([NO_DUMP_SHARE], 1,
                 [web2c: Define to disable architecture-independent dump files.
                  Faster on LittleEndian architectures.])])

AM_CONDITIONAL([FMU], [test "x$enable_fmtutil" = xyes])

KPSE_CHECK_WIN32
AM_CONDITIONAL([WIN32], [test "x$kpse_cv_have_win32" != xno])
AM_CONDITIONAL([MINGW32], [test "x$kpse_cv_have_win32" = xmingw32])

KPSE_CHECK_SOCKET_LIBS
AS_IF([test "x$enable_luatex:$enable_ipc" != xno:no],
      [AS_CASE([$ac_cv_search_connect],
               ["none required"], [],
               [no], [AS_IF([test "x$enable_luatex" = xyes],
                            [AC_MSG_WARN([Sorry, no socket library: can not build luaTeX])
                             enable_luatex=no])
                      AS_IF([test "x$enable_ipc" = xyes],
                            [AC_MSG_WARN([no socket library: disabling ipc])
                             enable_ipc=no])],
               [socketlibs="-l$ac_cv_search_connect"])])
AS_IF([test "x$enable_ipc" = xyes],
      [ipc_socketlibs=$socketlibs
       AC_DEFINE([IPC], 1, [tex: Define to enable --ipc.])])
AC_SUBST([socketlibs])
AC_SUBST([ipc_socketlibs])

KPSE_CHECK_FRAMEWORK([Carbon], [ATSUStyle style])
AS_IF([test "x$kpse_cv_have_Carbon" = xno],
      [KPSE_FONTCONFIG_FLAGS
       AS_IF([test "x$enable_xetex:$kpse_cv_have_fontconfig" = xyes:no],
             [AC_MSG_WARN([Sorry, can not build XeTeX without fontconfig])
              enable_xetex=no])])
AM_CONDITIONAL([XETEX_MACOSX], [test "x$kpse_cv_have_Carbon" = xyes])
AM_CONDITIONAL([XETEX_GRAPHITE], [test "x$with_graphite" != xno])

dnl Generate *TEX and ALEPH conditionals.
m4_foreach([Kpse_Prog], [kpse_tex_progs],
           [m4_ifset([Kpse_Prog],
                     [KPSE_XTEX_COND(Kpse_Prog)])])[]dnl

AS_IF([test "x$enable_aleph:$enable_xetex" = xno:no],
      [enable_otangle=no],
      [enable_otangle=yes])
AM_CONDITIONAL([OTANGLE],
               [test "x$enable_otangle" = xyes])[]dnl

AM_CONDITIONAL([OMFONTS], [true])

AM_CONDITIONAL([MF], [test "x$enable_mf" != xno])
AM_CONDITIONAL([MFN], [test "x$enable_mf_nowin" = xyes])

# Handle --with-mf-x-toolkit and --with-x
AS_CASE([$with_mf_x_toolkit],
        [''|yes|no], ,
        [''|y|ye],   [with_mf_x_toolkit=yes],
        [n],         [with_mf_x_toolkit=no],
        [AC_MSG_WARN([unknown --with-mf-x-toolkit argument \`$with_mf_x_toolkit'; using yes.])
         with_mf_x_toolkit=yes])
test "x$with_x_toolkit" = xyes && with_x=yes
test "x$with_x" = xno && with_x_toolkit=no
: ${with_x=no} # tell PATH_XTRA no X by default, if not specified.

# Distinguish no X at all, no toolkit, or toolkit.
AC_PATH_XTRA
if test "x$with_x" = xyes; then
  if test "x$no_x" = xyes; then
    AC_MSG_ERROR([Sorry, could not find required X include and library files.])
  fi
  test -n "$x_libraries" && XLFLAG="-L$x_libraries"
  wlibs="$XLFLAG -lX11"
  if test -z "$x_ext_lib"; then # allow envvar override
    AC_CHECK_LIB([Xext], [XextCreateExtension],
                 [x_ext_lib=-lXext], ,
                 [$wlibs $X_EXTRA_LIBS])
  fi
  test "x$with_mf_x_toolkit" = xyes && x_tool_libs="-lXt"
  AC_DEFINE([X11WIN], 1, [Define to include X11 window in Metafont.])
fi
AM_CONDITIONAL([MFXT], [test "x$with_mf_x_toolkit" = xyes])
AC_SUBST([x_ext_lib])
AC_SUBST([x_tool_libs])
AC_SUBST([wlibs])

dnl Generate MetaFont *WIN defines.
m4_foreach([Kpse_Win], [kpse_mf_win],
           [m4_ifset([Kpse_Win],
                     [KPSE_MFWIN_DEFINE(Kpse_Win)])])[]dnl

AM_CONDITIONAL([MP], [test "x$enable_mp" != xno])

AM_CONDITIONAL([WEB], [test "x$enable_web_progs" != xno])

dnl Although quite unusual, it is possible to build Web2C (TeX & Co)
dnl using installed (system) kpathsea headers and library.
dnl In that case we need the location of <kpathsea/paths.h>.
if test "x$with_system_kpathsea" = xyes; then
  if test "x$with_kpathsea_includes" = x; then
    list="/usr/include /usr/local/include"
  else
    list=$with_kpathsea_includes
  fi
  found=no
  for KPATHSEA_PATHS_H in $list; do
    if test -r "$KPATHSEA_PATHS_H/kpathsea/paths.h"; then
      found=yes
      break
    fi
  done
  if test "x$found" = xno; then
    AC_MSG_NOTICE([You requested to build `web2c' using an installed `kpathsea' version,])
    AC_MSG_NOTICE([    which requires to locate the <kpathsea/paths.h> header file.])
    AC_MSG_ERROR([Sorry, not found under any of: $list *****])
  fi
else
  KPATHSEA_PATHS_H='${top_builddir}/..'
fi
AC_SUBST([KPATHSEA_PATHS_H])

AC_PROG_YACC
KPSE_PROG_LEX

# Needed on A/UX 3.0. I don't want to pull in other -lposix's, though.
# From: bernt@weinberg.pop.bio.aau.dk (Bernt Guldbrandtsen)
if test `(uname) 2>/dev/null` = aux; then
  AC_CHECK_LIB([posix], [sigemptyset])
fi

dnl FIXME: luaTeX requires <unistd.h>, access, and atoi
dnl
AC_CHECK_SIZEOF([int])
AC_CHECK_SIZEOF([long])
AC_SEARCH_LIBS([pow], [m])
AC_CHECK_FUNCS([access atoi strerror])
AC_CHECK_HEADERS([errno.h langinfo.h sys/timeb.h sys/wait.h])
AC_TYPE_LONG_DOUBLE
AC_TYPE_LONG_LONG_INT

dnl FIXME: needed for libpdf -> xpdf ?
AC_CHECK_HEADERS([time.h])

dnl FIXME: obsolete
AC_TYPE_SIGNAL

KPSE_KPATHSEA_FLAGS
KPSE_ZLIB_FLAGS
KPSE_LIBPNG_FLAGS
KPSE_XPDF_FLAGS
KPSE_GRAPHITE_FLAGS
KPSE_FREETYPE2_FLAGS
KPSE_TECKIT_FLAGS
KPSE_ICU_FLAGS
KPSE_OBSDCOMPAT_FLAGS

dnl Write output here, instead of putting a zillion -D's on the command line.
AC_CONFIG_HEADERS([c-auto.h:c-auto.in],
 [sed -e 's/^#define PACKAGE/#define WEB2C_PACKAGE/' \
      -e 's/^#define VERSION/#define WEB2C_VERSION/' c-auto.h >c-auto.tmp && mv -f c-auto.tmp c-auto.h])

AH_TOP([/* c-auto.h: defines for web2c, as determined by configure.

   Copyright 1994-97, 2008, 2009 Karl Berry.
   Copyright 1997-99, 2002, 2005 Olaf Weber.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* Guard against double inclusion. */
#ifndef WEB2C_C_AUTO_H
#define WEB2C_C_AUTO_H

/* web2c: the version string. */
#define WEB2CVERSION "REPLACE-WITH-WEB2CVERSION"])
AH_BOTTOM([#endif /* !WEB2C_C_AUTO_H */])

dnl Additional config header for luaTeX
AC_CONFIG_HEADERS([ff-config.h:luatexdir/luafontloader/ff-config.in])

# For cross-compilation.  Put at the end so there is a fair chance
# these are still visible when the configure script has finished.

# The supporting programs we need to have available on the build machine.
#                     make variable  cross        native  
KPSE_CROSS_PATH_PROG([TANGLEBOOT],  [tangle],    [./tangleboot])
KPSE_CROSS_PATH_PROG([TANGLE],      [tangle],    [./tangle])
KPSE_CROSS_PATH_PROG([CTANGLEBOOT], [ctangle],   [./ctangleboot])
KPSE_CROSS_PATH_PROG([CTANGLE],     [ctangle],   [./ctangle])
KPSE_CROSS_PATH_PROG([TIE],         [tie],       [./tie])
dnl Required for Aleph and XeTeX
KPSE_CROSS_PATH_PROG([OTANGLE],     [otangle],   [./otangle])
dnl Required for luaTeX
KPSE_CROSS_PATH_PROG([LUATANGLE],   [luatangle], [./luatangle])

if test "x$cross_compiling" = xyes; then
  AC_MSG_CHECKING([if your tie supports WEBINPUTS])
  ( WEBINPUTS=$srcdir/tiedir
    export WEBINPUTS
    $TIE -c tie.outc tie.tie tie.cf1 tie.cf2 tie.cf3
    $TIE -m tie.outm tie.tie tie.cf1 tie.cf2 tie.cf3 ) >/dev/null 2>&1
  if diff tie.outc $srcdir/tiedir/tie.chf.gen >/dev/null 2>&1 && \
     diff tie.outm $srcdir/tiedir/tie.master.gen >/dev/null 2>&1; then
    tool_ok=yes
  else
    tool_ok=no
  fi
  AC_MSG_RESULT([$tool_ok])
  if test "x$tool_ok" != xyes; then
    AC_MSG_ERROR([Sorry, your $TIE is too old and does not support WEBINPUTS])
  fi
  rm -f tie.outc tie.outm
  if test "x$enable_otangle" = xyes; then
    AC_MSG_CHECKING([if your otangle supports WEBINPUTS])
    ( WEBINPUTS=$srcdir/cftests
      export WEBINPUTS
      $OTANGLE cftest cftest ) >/dev/null 2>&1
    if diff cftest.p $srcdir/cftests/ocftest.p >/dev/null 2>&1; then
      tool_ok=yes
    else
      tool_ok=no
    fi
    AC_MSG_RESULT([$tool_ok])
    if test "x$tool_ok" != xyes; then
      AC_MSG_ERROR([Sorry, your $OTANGLE is too old and does not support WEBINPUTS])
    fi
    rm -f cftest.p
  fi
  if test "x$enable_luatex" = xyes; then
    AC_MSG_CHECKING([if your luatangle supports WEBINPUTS])
    ( WEBINPUTS=$srcdir/cftests
      export WEBINPUTS
      $LUATANGLE cftest cftest ) >/dev/null 2>&1
    if diff cftest.p $srcdir/cftests/lcftest.p >/dev/null 2>&1; then
      tool_ok=yes
    else
      tool_ok=no
    fi
    AC_MSG_RESULT([$tool_ok])
    if test "x$tool_ok" != xyes; then
      AC_MSG_ERROR([Sorry, your $LUATANGLE is too old and does not support WEBINPUTS])
    fi
    rm -f cftest.p
  fi
fi

AC_CONFIG_SUBDIRS([web2c])

AC_CONFIG_FILES([Makefile doc/Makefile lib/Makefile man/Makefile
                 mpware/Makefile omegafonts/Makefile otps/Makefile
                 window/Makefile])

dnl The subdirectory web2c must be configured for the build system.
dnl Can not share the cache file with the subdirectory!
AC_CONFIG_COMMANDS_POST([AS_IF([test "x$cross_compiling" = xyes],
[cache_file=/dev/null
 ac_configure_args="$ac_configure_args --host='$build' \
 CC='$BUILDCC' CPPFLAGS='$BUILDCPPFLAGS'\
 CFLAGS='$BUILDCFLAGS' LDFLAGS='$BUILDLDFLAGS'"])])

AC_OUTPUT
