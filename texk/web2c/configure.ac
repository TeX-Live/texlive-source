dnl $Id$
dnl Process this file with Autoconf to produce a configure script for Web2c.
dnl
dnl   Copyright 2018-2019 Karl Berry <tex-live@tug.org>
dnl   Copyright 2009-2015 Peter Breitenlohner <tex-live@tug.org>
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
dnl get these features if you are willing to hack the sources.
dnl
m4_include([../../version.ac])[] dnl define tex_live_version
AC_INIT([Web2C], tex_live_version(), [tex-k@tug.org])
AC_PREREQ([2.65])
dnl 
dnl We don't use (for example) tex/tex.web because people who only want
dnl to build part of the distribution may not have any given program.
dnl Even cpascal.h isn't guaranteed, but then nothing is, really ...
AC_CONFIG_SRCDIR([cpascal.h])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIRS([../../m4 m4])

WEB2CVERSION=tex_live_version()
AC_SUBST([WEB2CVERSION])

KPSE_COMMON([web2c programs])

AM_CONDITIONAL([cross], [test "x$cross_compiling" = xyes])

AC_PROG_CXX
AC_PROG_OBJCXX
KPSE_CXX_HACK
KPSE_LT_HACK

# Include additional code for web2c.
KPSE_WEB2C_PREPARE
m4_include([ac/web2c.ac])

# XeTeX now requires C++11 because ICU does :(.
if test "x$enable_xetex" = xyes; then
  AC_MSG_NOTICE([checking for C++11, since XeTeX is enabled])
  AX_CXX_COMPILE_STDCXX([11])
fi

AS_IF([test "x$enable_texlive_build" = xyes],
      [banner_pre='TeX Live'],
      [banner_pre='Web2C'])
AC_DEFINE_UNQUOTED([WEB2CVERSION],
                   [" ($banner_pre $WEB2CVERSION$with_banner_add)"])

dnl For tests
AC_CHECK_PROGS([KPSEWHICH], [kpsewhich], [false])

KPSE_ASM_UNDERSCORE
KPSE_COND_MINGW32
KPSE_WIN32_CALL

AS_CASE([$with_editor],
        [''|yes|no], [AS_IF([test "x$kpse_cv_have_win32" = xno],
                            [with_editor="vi +%d '%s'"],
                            [with_editor='texworks --position=%d \"%s\"'])])
AC_DEFINE_UNQUOTED([EDITOR], ["$with_editor"],
                   [web2c: Default editor for interactive `e' option.])

AS_IF([test "x$enable_auto_core" = xyes],
      [AC_DEFINE([FUNNY_CORE_DUMP], 1,
                 [web2c: Define to enable HackyInputFileNameForCoreDump.tex.])])

AS_IF([test "x$enable_dump_share" = xno],
      [AC_DEFINE([NO_DUMP_SHARE], 1,
                 [web2c: Define to disable architecture-independent dump files.
                  Faster on LittleEndian architectures.])])

AS_CASE([$enable_ipc],
        [yes|no], [],
            [enable_ipc=yes])

KPSE_CHECK_SOCKET_LIBS
AS_CASE([$ac_cv_search_connect],
        ["none required"], [],
        [no], [WEB2C_DISABLE([luatex], [no socket library])
	       WEB2C_DISABLE([luatex53], [no socket library])
               WEB2C_DISABLE([ipc], [no socket library])],
            [socketlibs=$ac_cv_search_connect])
if test "x$enable_ipc" = xyes; then
   ipc_socketlibs=$socketlibs
   AC_DEFINE([IPC], 1, [tex: Define to enable --ipc.])
fi

# We may need additional libraries for LuaTeX.
# On Solaris -lnsl for gethostbyname(), -lrt for nanosleep(),
# and maybe -lresolv for inet_aton().
lua_socketlibs=$socketlibs
kpse_save_LIBS=$LIBS
KPSE_LUA53_DEFINES
KPSE_LUA52_DEFINES
KPSE_LUAJIT_DEFINES
LIBS=$kpse_save_LIBS
if test "x$kpse_cv_have_win32" = xno; then
  AC_SEARCH_LIBS([gethostbyname], [nsl])
  AC_SEARCH_LIBS([hstrerror], [resolv])
  AC_SEARCH_LIBS([inet_aton], [resolv])
  LIBS=$kpse_save_LIBS
  AC_SEARCH_LIBS([nanosleep], [rt])
  LIBS=$kpse_save_LIBS
  AS_CASE([$ac_cv_header_dlfcn_h],
          [yes], [AS_CASE([$ac_cv_search_dlopen],
                          [no*], [],
                          [lua_socketlibs="$lua_socketlibs $ac_cv_search_dlopen"])])
  AS_CASE([$ac_cv_search_hstrerror],
          ["none required"], [],
          [no], [WEB2C_DISABLE([luatex], [no hstrerror()]) WEB2C_DISABLE([luatex53], [no hstrerror()])],
              [lua_socketlibs="$lua_socketlibs $ac_cv_search_hstrerror"])
  AS_CASE([$ac_cv_search_inet_aton],
          ["none required"], [],
          [no], [WEB2C_DISABLE([luatex], [no inet_aton()]) WEB2C_DISABLE([luatex53], [no inet_aton()])],
              [lua_socketlibs="$lua_socketlibs $ac_cv_search_inet_aton"])
  AS_CASE([$ac_cv_search_gethostbyname],
          ["none required"], [],
          [no], [WEB2C_DISABLE([luatex], [no gethostbyname()]) WEB2C_DISABLE([luatex53], [no gethostbyname()])],
              [lua_socketlibs="$lua_socketlibs $ac_cv_search_gethostbyname"])
  AS_CASE([$ac_cv_search_nanosleep],
          ["none required"], [],
          [no], [WEB2C_DISABLE([luatex], [no nanosleep()]) WEB2C_DISABLE([luatex53], [no nanosleep()])],
              [lua_socketlibs="$lua_socketlibs $ac_cv_search_nanosleep"])
else
  AC_CHECK_LIB([ws2_32], [main],
               [lua_socketlibs="$socketlibs -lws2_32"],
               [WEB2C_DISABLE([luatex], [no getaddrinfo()]) WEB2C_DISABLE([luatex53], [no getaddrinfo()])])
fi
AM_CONDITIONAL([USE_DLOPEN], [test "x$ac_cv_header_dlfcn_h" = xyes])
AC_SUBST([lua_socketlibs])
AC_SUBST([ipc_socketlibs])

KPSE_CHECK_FRAMEWORK([ApplicationServices], [CTFontRef font])
KPSE_FONTCONFIG_FLAGS
AS_IF([test "x$kpse_cv_have_ApplicationServices:$kpse_cv_have_fontconfig" = xno:no],
      [WEB2C_DISABLE([xetex], [neither ApplicationServices framework nor fontconfig library])])
AM_CONDITIONAL([XETEX_MACOSX], [test "x$kpse_cv_have_ApplicationServices" = xyes])

dnl Generate *TEX and ALEPH conditionals.
m4_foreach([Kpse_Prog], [kpse_tex_progs],
           [m4_ifset([Kpse_Prog],
                     [KPSE_XTEX_COND(Kpse_Prog)])])[]dnl

test "x$enable_omfonts" = xno || enable_omfonts=yes

AS_IF([test "x$enable_aleph:$enable_xetex:$enable_omfonts" = xno:no:yes],
      [enable_otangle=no],
      [enable_otangle=yes])
AM_CONDITIONAL([OTANGLE],
               [test "x$enable_otangle" = xyes])[]dnl

# Handle --with-mf-x-toolkit and --with-x
AS_CASE([$with_mf_x_toolkit],
        [yes|no], [],
        [''], [with_mf_x_toolkit=no],
        [y|ye], [with_mf_x_toolkit=yes],
        [n], [with_mf_x_toolkit=no],
            [AC_MSG_WARN([unknown --with-mf-x-toolkit argument \`$with_mf_x_toolkit'; using no.])
             with_mf_x_toolkit=no])
test "x$with_x_toolkit" = xyes && with_x=yes
test "x$with_x" = xno && with_x_toolkit=no

dnl Generate MF* conditionals and set with_x.
m4_foreach([Kpse_Prog], [kpse_mf_progs],
           [m4_ifset([Kpse_Prog],
                     [KPSE_XMF_COND(Kpse_Prog)])])[]dnl
: ${with_x=yes}

# Distinguish no X at all, no toolkit, or toolkit.
AC_PATH_XTRA
if test "x$with_x" = xyes; then
  if test "x$no_x" = xyes; then
    AC_MSG_ERROR([Sorry, could not find X include and library files required for Metafont.])
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

AM_CONDITIONAL([WEB], [test "x$enable_web_progs" != xno])

AS_IF([test "x$enable_ptex:$enable_eptex" = xno:no],
      [enable_pweb_progs=no],
      [enable_pweb_progs=$enable_web_progs])
AM_CONDITIONAL([PWEB], [test "x$enable_pweb_progs" != xno])

AS_IF([test "x$enable_uptex:$enable_euptex" = xno:no],
      [enable_upweb_progs=no],
      [enable_upweb_progs=$enable_web_progs])
AM_CONDITIONAL([UPWEB], [test "x$enable_upweb_progs" != xno])

AC_PROG_YACC
KPSE_PROG_LEX

# Needed on A/UX 3.0. I don't want to pull in other -lposix's, though.
# From: bernt@weinberg.pop.bio.aau.dk (Bernt Guldbrandtsen)
if test `(uname) 2>/dev/null` = aux; then
  AC_CHECK_LIB([posix], [sigemptyset])
fi

AC_C_CHAR_UNSIGNED
AC_CHECK_SIZEOF([int])
if test $ac_cv_sizeof_int -lt 4; then
  AC_MSG_ERROR([Sorry, need `int' with at least 4 bytes.])
fi
AC_CHECK_SIZEOF([long])
AC_SEARCH_LIBS([pow], [m])
AC_CHECK_FUNCS([access atoi fmax ftime gettimeofday fabs labs mkdtemp setlocale strerror strlcat strlcpy strndup])
AC_CHECK_DECLS([strndup])
AC_CHECK_HEADERS([errno.h langinfo.h locale.h sys/time.h sys/timeb.h sys/wait.h time.h])
AC_CHECK_SIZEOF([void *])
AC_TYPE_UINTPTR_T
AC_TYPE_LONG_DOUBLE
AC_HEADER_STDBOOL

dnl sharable format files.
AC_C_BIGENDIAN

dnl FIXME: obsolete
AC_TYPE_SIGNAL

KPSE_KPATHSEA_FLAGS
KPSE_PTEXENC_FLAGS
KPSE_PPLIB_FLAGS
KPSE_ZLIB_FLAGS
KPSE_LUA53_FLAGS
KPSE_LUA52_FLAGS
KPSE_LUAJIT_FLAGS
KPSE_LIBPNG_FLAGS
KPSE_FREETYPE2_FLAGS
KPSE_PIXMAN_FLAGS
KPSE_CAIRO_FLAGS
KPSE_GMP_FLAGS
KPSE_MPFR_FLAGS
KPSE_XPDF_FLAGS
KPSE_ZZIPLIB_FLAGS
KPSE_TECKIT_FLAGS

# LuaTeX and XeTeX need harfbuzz.
if test "x$enable_xetex" = xyes \
   || test "x$enable_luatex" = xyes || test "x$enable_luatex53" = xyes; then
  KPSE_HARFBUZZ_FLAGS
fi # end xetex/luatex for harfbuzz

# XeTeX, but not LuaTeX, needs icu and graphite.
if test "x$enable_xetex" = xyes; then
  KPSE_ICU_FLAGS
  KPSE_GRAPHITE2_FLAGS
fi # end xetex for icu/graphite

# Check size of off_t (using <kpathsea/config.h>)
KPSE_ADD_FLAGS([kpathsea])
AC_CHECK_SIZEOF([off_t], , AC_INCLUDES_DEFAULT
[#include <kpathsea/config.h>])

if test "x$with_system_kpathsea" = xyes; then
dnl Although quite unusual, it is possible to build Web2C (TeX & Co)
dnl using installed (system) kpathsea headers and library.
dnl In that case we need the location of <kpathsea/paths.h>.
  list="/usr/include /usr/local/include `echo $KPATHSEA_INCLUDES | sed 's/-I//g'`"
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

dnl Check for Object::initCmd(const char *) etc
AC_LANG_PUSH([C++])
AC_MSG_CHECKING([for Object::[initCmd(const char*)] etc])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <Object.h>]],
                                   [[Object *obj; const char *name;
obj->initCmd(name)->isStream(name);]])],
                  [kpse_cv_have_Object_initCmd_const_charp=yes],
                  [kpse_cv_have_Object_initCmd_const_charp=no])
AC_MSG_RESULT([$kpse_cv_have_Object_initCmd_const_charp])
AC_LANG_POP([C++])
if test "x$kpse_cv_have_Object_initCmd_const_charp" = xyes; then
  AC_DEFINE([HAVE_OBJECT_INITCMD_CONST_CHARP], 1,
            [Define to 1 if you have the `Object::initCmd(const char*)' function.])
fi
KPSE_RESTORE_FLAGS

dnl Write output here, instead of putting a zillion -D's on the command line.
AC_CONFIG_HEADERS([w2c/c-auto.h:c-auto.in],
 [sed -e 's/^#define PACKAGE/#define WEB2C_PACKAGE/' \
      -e 's/^#define VERSION/#define WEB2C_VERSION/' w2c/c-auto.h >c-auto.tmp \
    && mv -f c-auto.tmp w2c/c-auto.h])

AH_TOP([/* w2c/c-auto.h: defines for web2c, as determined by configure.

   Copyright 1994-97, 2008-2012 Karl Berry.
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

dnl Additional config header for LuaTeX
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
fi

AC_CONFIG_FILES([Makefile doc/Makefile lib/Makefile man/Makefile
                 omegafonts/Makefile otps/Makefile otps/win32/Makefile
                 window/Makefile])
AC_CONFIG_FILES([web2c-sh], [chmod +x web2c-sh])
AC_CONFIG_FILES([tangle-sh], [chmod +x tangle-sh])
AC_CONFIG_FILES([ctangleboot-sh], [chmod +x ctangleboot-sh])
AC_CONFIG_FILES([silent-sh], [chmod +x silent-sh])

dnl Additional code for synctex
m4_sinclude([synctexdir/ac/synctex.ac])

dnl The subdirectory web2c must be configured for the build system.
KPSE_NATIVE_SUBDIRS([web2c])

AC_OUTPUT
