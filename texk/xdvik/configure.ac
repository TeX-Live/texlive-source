dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009-2014 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_define([xdvik_version], [22.87.04])
AC_INIT([xdvik], xdvik_version, [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([xdvi.c])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIRS([../../m4 m4])

dnl Common code for all programs using libkpathsea.
KPSE_COMMON([xdvik])
KPSE_CANONICAL_HOST

dnl Include additional code for xdvik.
m4_include([ac/xdvik.ac])

# Reduce compiler warnings, there are just too many.
WARNING_CFLAGS=`echo $WARNING_CFLAGS | sed -e 's/ -Wcast-qual/ /' -e 's/ -Wwrite-strings/ /'`

AC_SEARCH_LIBS([pow], [m])

AC_ARG_WITH([ps],
            AS_HELP_STRING([--with-ps=PS],
                           [Use PostScript (PS=no/yes/gs/dps/news, default gs)]))
AS_CASE([$with_ps],
        ["" | yes | gs], [PS_DEF=-DPS_GS],
        [no],            [PS_DEF=],
        [dps],           [PS_DEF=-DPS_DPS],
        [news],          [PS_DEF=-DPS_NEWS],
                         [AC_MSG_WARN([unknown--with-ps argument `$with_ps'; using gs.])
                          PS_DEF=-DPS_GS])
AC_SUBST([PS_DEF])

dnl Required, not optional.
dnl AC_ARG_ENABLE([grey],
dnl               AS_HELP_STRING([--disable-grey],
dnl                              [Disable greyscale anti-aliasing for shrunken bitmaps]))
AC_DEFINE([GREY], 1, [Define to enable greyscale anti-aliasing for shrunken bitmaps.])

dnl Required, not optional.
dnl AC_ARG_ENABLE([color],
dnl               AS_HELP_STRING([--disable-color],
dnl                              [Disable support for color specials]))
AC_DEFINE([COLOR], 1, [Define to enable support for color specials.])

dnl Required, not optional.
dnl AC_ARG_ENABLE([freetype],
dnl               AS_HELP_STRING([--disable-freetype],
dnl                              [Do not use FreeType2 (direct rendering of Type1 fonts)]))
AC_DEFINE([FREETYPE], 1, [Define to compile in the FreeType2 library.])

dnl OBSOLETE AC_ARG_ENABLE([gf],
dnl OBSOLETE               AS_HELP_STRING([--enable-gf],
dnl OBSOLETE                              [Enable gf format pixel files (in addition to pk)]))
dnl OBSOLETE AC_DEFINE([USE_GF], 1, [Define to enable gf format files (in addition to pk format).])

AC_ARG_ENABLE([a4],
              AS_HELP_STRING([--disable-a4],
                             [Set default paper size to letter and default unit to inch]))
AS_IF([test "x$enable_a4" = xno],
      [AC_MSG_NOTICE([Disabled a4/cm, using letter/in instead])],
      [AC_DEFINE([A4], 1, [Define to use A4 as the default paper size.])])

AC_CONFIG_HEADERS([c-auto.h:c-auto.in])

AC_FUNC_FORK
AC_FUNC_VPRINTF
XDVI_FUNC_WORKING_VSNPRINTF
XDVI_FUNC_MEMICMP
AC_FUNC_ALLOCA
AC_C_STRINGIZE
AC_FUNC_MEMCMP
AC_HEADER_SYS_WAIT
AC_TYPE_SIGNAL
AC_C_BIGENDIAN
AC_PATH_XTRA

dnl XDVI checks by Paul Vojta
XDVI_FUNC_SETSID_IN_VFORK
XDVI_C_BITMAP_TYPE
XDVI_SYS_STREAMS
XDVI_SYS_OLD_LINUX
XDVI_FUNC_POLL
XDVI_FUNC_XKB_BELL
XDVI_XINPUT_2_1

dnl #######

dnl COMPILER_WARNINGS

dnl #######

# Check for `--allow-multiple-definition' linker flag
XDVI_LINKER_MULTIPLE_DEFNS

AC_CHECK_HEADERS([stdint.h inttypes.h])
AC_TYPE_UINT32_T
AC_CHECK_TYPE([ptrdiff_t], [],
              [AC_DEFINE_UNQUOTED([ptrdiff_t], [long],
                                  [Define to `long' if <sys/types.h> does not define.])])

AC_CHECK_FUNCS([mkstemp memcpy strerror waitpid sigaction strchr setenv \
                fchdir getcwd getpwnam getuid getpwuid \
                lstat ftruncate snprintf vsnprintf realpath])

# <sys/bsdtypes.h> is for ISC 4.0, to define fd_set in psgs.c.
# regex.h is needed for regexp support in DVI search.    
AC_CHECK_HEADERS([netdb.h sys/bsdtypes.h sys/param.h unistd.h sys/fcntl.h regex.h])

# Checks for iconv
XDVI_CHECK_ICONV

# Checks for langinfo
XDVI_CHECK_LANGINFO

# Check X11 headers.
# X11/IntrinsicI.h is needed for LateBindings (to roll our own translations)
xdvi_save_CPPFLAGS=$CPPFLAGS
CPPFLAGS="$CPPFLAGS $X_CFLAGS"
AC_CHECK_HEADERS([X11/Intrinsic.h X11/IntrinsicI.h X11/Xosdefs.h X11/xpm.h \
		  Xm/XpmP.h])
# For editres, we need to check both the header and define a linker option.
AC_CHECK_HEADERS([X11/Xmu/Editres.h X11/Xmu/Xmu.h],
                 [x_xmu_lib=-lXmu],
                 [x_xmu_lib=],
                 [#ifdef HAVE_X11_INTRINSIC_H
# include <X11/Intrinsic.h>
#endif])
AC_SUBST([x_xmu_lib])
CPPFLAGS=$xdvi_save_CPPFLAGS

AC_ARG_ENABLE([magick],
              AS_HELP_STRING([--enable-magick],
                             [Use ImageMagick to load/render image files.
                              EXPERIMENTAL - DO NOT USE!]))
if test "x$enable_magick" = xyes; then
  AC_MSG_CHECKING([for ImageMagick version])
  if MAGICK_VER=`Magick-config --version 2>/dev/null`; then
    AC_MSG_RESULT([$MAGICK_VER])
    set dummy `echo $MAGICK_VER | sed -e 's/\./ /g'`
    MAGICK_VER_MAJOR=$2
    MAGICK_VER_MINOR=$3
    if expr $MAGICK_VER_MAJOR \> 4 >/dev/null && \
      (expr $MAGICK_VER_MAJOR \> 5 >/dev/null || \
	expr $MAGICK_VER_MINOR \> 2 >/dev/null); then
      MAGICK_CPP="-DMAGICK_VER_MAJOR=$MAGICK_VER_MAJOR -DMAGICK_VER_MINOR=$MAGICK_VER_MINOR `Magick-config --cppflags`"
      MAGICK_LD="`Magick-config --libs` `Magick-config --ldflags`"
      AC_MSG_NOTICE([ImageMagick support enabled])
      X_CFLAGS="$X_CFLAGS $MAGICK_CPP"
      X_EXTRA_LIBS="$X_EXTRA_LIBS $MAGICK_LD"
      AC_DEFINE([MAGICK], 1, [Define to use ImageMagick])
    else
      AC_MSG_NOTICE([ImageMagick version too old - disabled])
    fi
  else
    AC_MSG_RESULT([cannot run 'Magick-config' - ImageMagick support disabled])
  fi
fi

# Check whether -lXpm can be used	
XDVI_FIND_XPM

# Check for libXext.
AS_IF([test -n "$x_libraries"],
      [XLFLAG="-L$x_libraries"],
      [XLFLAG=])
if test -z "$x_ext_lib"; then # allow envvar override
  AC_CHECK_LIB([Xext], [XextCreateExtension],
               [x_ext_lib='-lXext'], [],
               [-lX11 $X_EXTRA_LIBS $XLFLAG])
fi
AC_SUBST([x_ext_lib])

# Check for libXp.
# Although libXp is now defunct (no longer part of XOrg), older versions
# of libXm may need to link with libXp; same for the equally defunct libXaw8.
# We certainly don't want to include libXp unless necessary, since this might
# only perpetuate the need to keep libXp.so around.
# Moreover for ELF shared libraries a requirement for libXp.so is probably
# encoded into libXm.so and/or libXaw.so and need not be specified explicitly.
AC_CHECK_LIB([Xp], [XpQueryVersion],
             [x_xp_lib='-lXp'], [],
             [$x_ext_lib -lX11 $X_EXTRA_LIBS $XLFLAG])

if test "x$enable_build" != xno || test -f config.force; then

# Check which toolkit was specified
AS_CASE([$with_xdvi_x_toolkit],
        [next | nextaw | neXtaw],      [val=neXtaw],
        [xaw3d],                       [val=xaw3d],
        ["" | x | xa | xaw],           [val=xaw],
        [y | ye | yes],                [val=yes],
        [m | mo | mot | moti | motif], [val=motif],
                                       [AC_MSG_WARN([unknown --with-xdvi-x-toolkit argument 
                                                       `$with_xdvi_x_toolkit'; using xaw.])
                                        val=xaw])
with_xdvi_x_toolkit=$val

if test "x$with_xdvi_x_toolkit" = xxaw3d; then
  prog_extension="xaw3d"
  x_tool_libs="-lXaw3d"
elif test "x$with_xdvi_x_toolkit" = xneXtaw; then
  prog_extension="nextaw"
  x_tool_libs="-lneXtaw"
else
  # Check for Xaw headers and library version
  XDVI_CHECK_XAW_HEADERS
  if test "x$with_xdvi_x_toolkit" != xxaw; then
    if test "x$with_xdvi_x_toolkit" = xmotif; then
      # Requested motiv
      default_toolkit=none
    else
      # Requested motiv with Xaw as fallback
      default_toolkit=Xaw
    fi
    XDVI_FIND_MOTIF
  fi
  if test "x$with_xdvi_x_toolkit" = xxaw; then
    # Requested or fallback to Xaw, check for library
    XDVI_CHECK_XAW_LIBRARY
  fi
fi
AC_SUBST([x_tool_libs])
final_exec_name="xdvi-$prog_extension"
program_transform_name="'s,xdvi-bin,$final_exec_name,'"
AC_SUBST([final_exec_name])

dnl #######

dnl $program_transform_name replacement

dnl #######

# If we're on Solaris and we want DPS, then add the extra compilation stuff
# to get it.
if echo "$PS_DEF" | grep PS_DPS >/dev/null; then
   if test "`(uname) 2>/dev/null`" = SunOS \
      && uname -r | grep '^5' >/dev/null; then
    AC_MSG_NOTICE([adding DPS includes and libraries for Solaris])
    X_CFLAGS="$X_CFLAGS -I/usr/openwin/include/X11"
    X_LIBS="$X_LIBS -R/usr/openwin/lib /usr/openwin/lib/libdps.so $ac_sunmath"
    AC_CHECK_LIB([sunmath], [isinf], 
                 [X_LIBS="$X_LIBS -lsunmath"])
    dnl We only need isinf and ieeefp.h to work around a Solaris bug in
    dnl -ldps; see psdps.c.  isinf is in -lsunmath.
    AC_CHECK_FUNCS([isinf])
    AC_CHECK_HEADERS([ieeefp.h])
  else
    dnl SunOS 4.x is simpler.
    AC_MSG_NOTICE([adding DPS libraries for SunOS 4.x])
    X_LIBS="$X_LIBS -ldps"
  fi
elif echo "$PS_DEF" | grep PS_NEWS >/dev/null; then
    AC_MSG_NOTICE([adding NeWS libraries])
    X_LIBS="$X_LIBS -lxview -lcps -lolgx -lxvps"
fi

KPSE_KPATHSEA_FLAGS
KPSE_FREETYPE2_FLAGS

XDVI_GS_LIB_PATH

KPSE_ADD_FLAGS([kpathsea])

KPSE_CHECK_KPSE_FORMAT([enc], [],
                       [AC_MSG_ERROR([your kpathsea has no kpse_enc_format.])])
AC_CHECK_HEADER([kpathsea/version.h], [],
                [AC_MSG_ERROR([Sorry, kpathsea too old])])

KPSE_RESTORE_FLAGS

echo timestamp >config.force
fi

AC_ARG_WITH([default-dvips-path],
            AS_HELP_STRING([--with-default-dvips-path=CMD],
                           [Use CMD as default DVI to PS converter]), ,
            [with_default_dvips_path="dvips"])
AC_DEFINE_UNQUOTED([DEFAULT_DVIPS_PATH], ["$with_default_dvips_path"],
                   [Define to point to the default command to use for printing (optional).])

AC_ARG_WITH([default-ps2pdf-path],
            AS_HELP_STRING([--with-default-ps2pdf-path=CMD],
                           [Use CMD as default PS to PDF converter]), ,
            [with_default_ps2pdf_path="ps2pdf"])
AC_DEFINE_UNQUOTED([DEFAULT_PS2PDF_PATH], ["$with_default_ps2pdf_path"],
                   [Define to point to the default command to use for printing (optional).])

XDVIK_VERSION=xdvik_version
AC_SUBST([XDVIK_VERSION])

AC_CONFIG_FILES([Makefile tests/Makefile])

AC_CONFIG_FILES([xdvi:xdvi-sh.in], [chmod +x xdvi])

dnl The subdirectory squeeze must be configured for the build system.
KPSE_NATIVE_SUBDIRS([squeeze])

AC_OUTPUT
