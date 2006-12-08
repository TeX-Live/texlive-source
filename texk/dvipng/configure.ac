# Thomas Esser, for teTeX. All settings here are my fault, don't blame
# the dvipng maintainers for it.

# Process this file with autoconf to produce a configure script.
AC_INIT([dvipng], [1.9], [dvipng@nongnu.org])
AC_CONFIG_SRCDIR([dvipng.c])

AC_ARG_ENABLE(debug, 
  AC_HELP_STRING([--disable-debug],[Compile without debug (-d) option]), 
    [ if test "$enableval" = yes ; then
      AC_DEFINE(DEBUG, 1, [Define as 1 to get the debug (-d) option.])
      fi ],	
    [ enable_debug="yes";
      AC_DEFINE(DEBUG, 1, [Define as 1 to get the debug (-d) option.])])
AC_ARG_ENABLE(timing, 
  AC_HELP_STRING([--enable-timing],[Output execution time of dvipng]), 
   [ if test "$enableval" = yes ; then
     AC_DEFINE(TIMING, 1, [Define as 1 to get execution time output.])
     fi ])

# Checks for programs.
AC_SET_MAKE
AC_PROG_CC
AC_PROG_INSTALL
AC_ARG_WITH(gs,
  AC_HELP_STRING([--with-gs=/PATH/TO/gs],[Hard-wire the location of GhostScript]), 
  [if test "x$withval" = xno ; then
    GS=no
  else
    AC_PATH_PROG(GS,["$withval"])
  fi],
  [AC_CHECK_PROG(GS,gs,gs)
  if test -z "$GS"; then
    GS="gs"
    AC_MSG_WARN([Cannot find GhostScript in your PATH])
  fi
])
AC_DEFINE_UNQUOTED(GS_PATH, "$GS", [Define as the path to GhostScript.])


dnl Check devices for GS
dnl AC_GS_HAS_DEVICE(DEVICE,ACTION-IF-FAILED)
dnl
AC_DEFUN(AC_GS_HAS_DEVICE,
 [AC_MSG_CHECKING([whether $GS has the $1 device])
  if $GS -h | grep $1 >/dev/null; then
    AC_MSG_RESULT(yes)
  else
    AC_MSG_RESULT(no)
    $2
  fi
])

GS_WARN=""
AC_GS_HAS_DEVICE(pngalpha,
   [GS_WARN="Your EPS inclusions will be cropped to the 
             boundingbox, and rendered on an opaque background. 
             Upgrade GhostScript to avoid this."
   AC_GS_HAS_DEVICE(png16m,
      [GS_WARN="Your EPS inclusions may not work.
             Upgrade/install GhostScript to avoid this."])])
if test -n "$GS_WARN"; then
  AC_MSG_WARN([$GS_WARN])
fi

ac_cv_lib_t1_T1_InitLib=yes
PSFONTS_O=""
if test "$ac_cv_lib_t1_T1_InitLib" = yes; then
  PSFONTS_O="t1.o"
fi
AC_SUBST(PSFONTS_O)

ac_have_freetype2="yes"    # we have ft2
if test "$ac_have_freetype2" = yes; then
  # Have to set compiler and linker flags in Makefile.in, it seems.
	PSFONTS_O="$PSFONTS_O sfd.o ft.o"	
	AC_DEFINE(HAVE_FT2, 1, [Define to 1 if you have freetype2])
	AC_DEFINE(HAVE_FT_LIBRARY_VERSION)
fi

if test -n "$PSFONTS_O"; then
  PSFONTS_O="$PSFONTS_O enc.o fontmap.o tfm.o"
fi
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_HEADER_TIME
AC_CHECK_HEADERS([inttypes.h fcntl.h sys/time.h stdbool.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_PID_T
AC_TYPE_SIZE_T

#if test "$ac_cv_header_inttypes_h" = yes; then
  # Sometimes we want to use gcc -ansi -pedantic as a portability test
  # The typedef of int64_t is not in the system header file in that
  # case. Then, #define int64_t as "long long", which is non-ansi, but
  # is present in most modern compilers. Using a #define rather than a
  # typedef can be a problem, but in dvipng int64_t only is used as
  # typecast, and there are no problems.

#  MY_CHECK_TYPE(int64_t, long long)
#  MY_CHECK_TYPE(uint64_t, unsigned long long)
#fi

# Checks for library functions.
AC_FUNC_ALLOCA
AC_FUNC_MMAP
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([dup2 memset munmap pow putenv strchr strrchr strtol strstr])
if test "$enable_timing" = "yes"; then
  AC_CHECK_FUNCS([ftime gettimeofday])
fi

# let's skip dvigif for the moment.
INSTALL_BIN_TARGET="install-dvipng"
AC_SUBST(INSTALL_BIN_TARGET)


dnl
dnl MAKEINFO_CHECK_MACRO( MACRO, [ACTION-IF-FOUND 
dnl					[, ACTION-IF-NOT-FOUND]])
dnl
AC_DEFUN(MAKEINFO_CHECK_MACRO,
[if test -n "$MAKEINFO" -a "$makeinfo" != ":"; then
  AC_MSG_CHECKING([if $MAKEINFO understands @$1{}])
  echo \\\\input texinfo > conftest.texi
  echo @$1{test} >> conftest.texi
  if $MAKEINFO conftest.texi > /dev/null 2> /dev/null; then
    AC_MSG_RESULT(yes)	
    ifelse([$2], , :, [$2])
  else  
    AC_MSG_RESULT(no)	
    ifelse([$3], , :, [$3])
  fi
  rm -f conftest.texi conftest.info
fi
])

dnl
dnl MAKEINFO_CHECK_MACROS( MACRO ... [, ACTION-IF-FOUND 
dnl					[, ACTION-IF-NOT-FOUND]])
dnl
AC_DEFUN(MAKEINFO_CHECK_MACROS,
[for ac_macro in $1; do
    MAKEINFO_CHECK_MACRO($ac_macro, $2, 
	[MAKEINFO_MACROS="-D no-$ac_macro $MAKEINFO_MACROS"
	$3])dnl
  done
AC_SUBST(MAKEINFO_MACROS)
])

# Documentation-related checks
AC_PATH_PROG(MAKEINFO, makeinfo, :)
MAKEINFO_CHECK_MACROS(acronym env option)
AC_PATH_PROG(INSTALL_INFO, install-info, :, $PATH /usr/sbin /sbin)

sinclude(../../libs/zlib/withenable.ac)
sinclude(../../libs/zlib/zlib.ac)
sinclude(../../libs/libpng/withenable.ac)
sinclude(../../libs/libpng/libpng.ac)
sinclude(../../libs/t1lib/withenable.ac)
sinclude(../../libs/t1lib/t1lib.ac)
sinclude(../../libs/gd/withenable.ac)
sinclude(../../libs/gd/gd.ac)
sinclude(../../libs/freetype2/withenable.ac)
sinclude(../../libs/freetype2/freetype2.ac)
sinclude(../kpathsea/withenable.ac)

dnl -- gd jpeg fn not defined? AC_DEFINE(HAVE_GDIMAGECREATETRUECOLOR)
dnl then we get a warning about gdImageAlpha, but so what.
AC_DEFINE(HAVE_GDIMAGEPNGEX)
AC_DEFINE(HAVE_GD_H)
AC_DEFINE(HAVE_LIBGD)

AC_DEFINE(HAVE_T1LIB_H)
AC_DEFINE(HAVE_LIBT1)

AC_DEFINE(HAVE_PNG_H)
AC_DEFINE(HAVE_LIBPNG)

AC_DEFINE(HAVE_LIBZ)

AC_CHECK_LIB(m, main)
AC_CHECK_LIB(gen, basename)

LIBS="../kpathsea/.libs/libkpathsea.a $LIBS"
AC_DEFINE(HAVE_KPATHSEA_KPATHSEA_H)
AC_DEFINE(HAVE_KPSE_ENC_FORMATS)
AC_DEFINE(HAVE_LIBKPATHSEA)

#
# Check for ApplicationServices framework (Mac OS X)
#
AC_DEFUN([AC_HAS_APP_SERVICES], [
 _cppflags=$CPPFLAGS _ldflags=$LDFLAGS
 AC_MSG_CHECKING([for Mac OS X ApplicationServices framework])
 AC_TRY_COMPILE(
 [
#include <ApplicationServices/ApplicationServices.h>
 ], [
ATSFontRef fontRef;
 ],
 [ AC_MSG_RESULT(yes)
   AC_DEFINE(HAVE_APP_SERVICES, 1, [Define if you have ApplicationServices (Mac OS X)])
   LIBS="$LIBS -framework ApplicationServices"
 ],
 [ AC_MSG_RESULT(no) ]
 )
])

AC_HAS_APP_SERVICES

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES(Makefile)
AC_OUTPUT
