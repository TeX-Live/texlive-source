# configure.ac

#************************************************************************
#
#  Part of the dvipng distribution
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU Lesser General Public License as
#  published by the Free Software Foundation, either version 3 of the
#  License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful, but
#  WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this program. If not, see
#  <http://www.gnu.org/licenses/>.
#
#  Copyright (C) 2002-2015,2019 Jan-Åke Larsson
#
#************************************************************************

# Process this file with autoconf to produce a configure script.
AC_INIT([dvipng], [1.17], [dvipng@nongnu.org])
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
AC_PROG_LN_S
AC_ARG_WITH(gs,
  AC_HELP_STRING([--with-gs=/PATH/TO/gs],[Hard-wire the location of GhostScript]),
  [if test "x$withval" != xno ; then
    AC_PATH_PROG(GS,["$withval"])
    AC_DEFINE_UNQUOTED(GS_PATH, "$GS", [Define as the path to GhostScript.])
    GS_CHECK_DEVICES
  fi],
  [AC_CHECK_PROG(GS,gs,gs)
  AC_DEFINE_UNQUOTED(GS_PATH, "gs", [Define as the path to GhostScript.])
  if test -n "$GS"; then
    GS_CHECK_DEVICES
  else
    AC_MSG_WARN([Cannot find GhostScript in your PATH])
  fi
])

# Checks for libraries.
AC_CHECK_LIB(m, pow)
AC_CHECK_LIB(z, deflate)
AC_CHECK_LIB([png], [png_read_image],,
             [AC_MSG_ERROR([cannot find/use libpng])])
AC_CHECK_LIB([gd], [gdImageCreate],,
             [AC_MSG_ERROR([cannot find/use libgd
This drawing library can be downloaded at
https://bitbucket.org/libgd/gd-libgd/downloads])])
AC_CHECK_LIB([kpathsea], [kpse_set_program_name],,
             AC_MSG_ERROR([cannot find/use libkpathsea]))

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([gd.h png.h kpathsea/kpathsea.h],,
		 [AC_MSG_ERROR([cannot find/use $ac_header])])
AC_CHECK_HEADERS([libgen.h])
PSFONTS_O=""
AC_SUBST(PSFONTS_O)
PKG_CHECK_MODULES([FT2], [freetype2 >= 6.1.0],
	[CFLAGS="$FT2_CFLAGS $CFLAGS"
	LIBS="$FT2_LIBS $LIBS"
	PSFONTS_O="sfd.o ft.o enc.o fontmap.o tfm.o"
	AC_DEFINE(HAVE_FT2, 1, [Define to 1 if you have freetype2])
	ac_have_freetype2="yes"],
	[ac_have_freetype2="no"])
AC_HEADER_SYS_WAIT
AC_HEADER_TIME
AC_HEADER_STDBOOL
AC_CHECK_HEADERS([fcntl.h sys/time.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_PID_T
AC_TYPE_SIZE_T
if test "$ac_cv_header_inttypes_h" = "yes"; then
  # Sometimes we want to use gcc -ansi -pedantic as a portability test
  # The typedef of int64_t is not in the system header file in that
  # case. Then, #define int64_t as "long long", which is non-ansi, but
  # is present in most modern compilers. Using a #define rather than a
  # typedef can be a problem, but in dvipng int64_t is only used as
  # typecast, and there are no problems. autoconf 2.13 equivalent:
  # AC_CHECK_TYPE(int64_t, long long)
  # AC_CHECK_TYPE(uint64_t, unsigned long long)
  AC_CHECK_TYPE([int64_t],,
                [AC_DEFINE_UNQUOTED([int64_t], [long long],
                                    [Define to `long long' if
                                     <inttypes.h> does not define it.])])
  AC_CHECK_TYPE([uint64_t],,
                [AC_DEFINE_UNQUOTED([uint64_t], [unsigned long long],
                                    [Define to `unsigned long long' if
                                     <inttypes.h> does not define it.])])
fi
AC_HAS_KPSE_ENC_FORMATS


# Checks for library functions.
AC_FUNC_FORK
# We always allocate nonzero chunks
# AC_FUNC_MALLOC
AC_FUNC_MMAP
# AC_FUNC_REALLOC
AC_FUNC_STRTOD
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([dup2 memset munmap pow putenv strchr strrchr strtol strstr strncasecmp])
if test "$enable_timing" = "yes"; then
  AC_CHECK_FUNCS([ftime gettimeofday])
fi
AC_CHECK_FUNCS([gdImageCreateTrueColor gdImageCreateFromJpeg gdImagePngEx gdImageCreateFromPngPtr gdImageGif FT_Library_Version])
if test "$ac_cv_func_gdImageGif" = "yes"; then
   INSTALL_BIN_TARGET="install-dvigif"
else
   INSTALL_BIN_TARGET="install-dvipng"
fi
AC_CHECK_FUNCS([texlive_gs_init])
AC_SUBST(INSTALL_BIN_TARGET)

# Documentation-related checks
AC_PATH_PROG(MAKEINFO, makeinfo, :)
MAKEINFO_CHECK_MACROS(acronym env option)
AC_PATH_PROG(INSTALL_INFO, install-info, :, $PATH /usr/sbin /sbin)

# SELFAUTO
AC_ARG_ENABLE(selfauto-set,
  AC_HELP_STRING([--enable-selfauto-set],
  [This option will make the final binary explicitly set the
   $SELFAUTO... variables to make it look as dvipng is installed in the
   main texmf tree, even if it isn't. This is necessary when texmf.cnf
   only uses $SELFAUTO... variables and dvipng is not installed in the
   texmf tree. Otherwise, dvipng may not be able to find virtual
   fonts, or psfonts.map. To find out, first build the binary and do
   'make test'. If the test fails, you need this switch.]),
  [ if test "$enableval" = yes ; then
      AC_MSG_CHECKING([for \$SELFAUTOLOC])
      SELFAUTOLOC=`kpsewhich -expand-var=\\\$SELFAUTOLOC`
      AC_DEFINE_UNQUOTED(ENV_SELFAUTOLOC,["SELFAUTOLOC=$SELFAUTOLOC"],
	[The environment setting for $SELFAUTOLOC])
      AC_MSG_RESULT([$SELFAUTOLOC])
      AC_MSG_CHECKING([for \$SELFAUTODIR])
      SELFAUTODIR=`kpsewhich -expand-var=\\\$SELFAUTODIR`
      AC_DEFINE_UNQUOTED(ENV_SELFAUTODIR,["SELFAUTODIR=$SELFAUTODIR"],
	[The environment setting for $SELFAUTODIR])
      AC_MSG_RESULT([$SELFAUTODIR])
      AC_MSG_CHECKING([for \$SELFAUTOPARENT])
      SELFAUTOPARENT=`kpsewhich -expand-var=\\\$SELFAUTOPARENT`
      AC_DEFINE_UNQUOTED(ENV_SELFAUTOPARENT,["SELFAUTOPARENT=$SELFAUTOPARENT"],
	[The environment setting for $SELFAUTOPARENT])
      AC_MSG_RESULT([$SELFAUTOPARENT])
    fi ],
  [AC_MSG_CHECKING([for texmf.cnf])
  TEXMF_CNF=`kpsewhich texmf.cnf`
  AC_MSG_RESULT([$TEXMF_CNF])
  AC_PATH_PROG(KPSEWHICH, kpsewhich)
  AC_MSG_CHECKING([for psfonts.map])
  cp $KPSEWHICH .
  PSFONTS_MAP=`./kpsewhich psfonts.map`
  rm -f ./kpsewhich
  if test -n "$PSFONTS_MAP"; then
    AC_MSG_RESULT([$PSFONTS_MAP])
  else
    AC_MSG_RESULT([not found from outside the texmf tree])
    AC_MSG_CHECKING([for \$SELFAUTO in texmf.cnf])
    if grep SELFAUTO "$TEXMF_CNF" > /dev/null 2> /dev/null; then
      AC_MSG_RESULT([yes
***************************************************************
texmf.cnf is using \$SELFAUTO... variables. If you are going to
install dvipng outside the texmf tree, you may need to use
--enable-selfauto-set. To find out, do 'make ; make test'. If the test
is unsuccessful, add the mentioned switch and rebuild.
***************************************************************])
    else
      AC_MSG_RESULT([no])
    fi
  fi])

AC_MSG_RESULT([
** Configuration summary for $PACKAGE_STRING:

   The -d (debug) switch is enabled:        $enable_debug
   Your gd is new enough (>=2.0) to enable
     the --truecolor switch, full alpha
     transparency, proper rescaling of
     included bitmaps, and jpeg inclusion:  $ac_cv_func_gdImageCreateTrueColor
   Your gd has jpeg inclusion enabled:      $ac_cv_func_gdImageCreateFromJpeg
   Your gd is new enough (>=2.0.12) to
     enable transparent backgrounds for EPS
     inclusion and the -z (compression)
     switch:                                $ac_cv_func_gdImagePngEx
   Your gd is new enough (>=2.0.21) to
     allow image creation from memory       $ac_cv_func_gdImageCreateFromPngPtr
   Your gd is new enough (>=2.0.28) to
     enable gif inclusion and output
     (dvigif):                              $ac_cv_func_gdImageGif
   FreeType font rendering available:       $ac_have_freetype2
   Support for subfonts (CJK-LaTeX):        $ac_have_freetype2
   Building within TeX Live:                $ac_cv_func_texlive_gs_init
])

AC_CONFIG_HEADER([config.h])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
