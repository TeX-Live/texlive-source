dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_define([xdvik_version], [22.84.15])
AC_INIT([xdvik for TeX Live], xdvik_version, [tex-k@tug.org])
AC_PREREQ([2.63])
AC_CONFIG_SRCDIR([xdvi.c])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

dnl Common code for all programs using libkpathsea.
KPSE_COMMON([xdvik])

m4_include([ac/xdvik.ac])
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

dnl OBSOLETE AC_ARG_ENABLE([buttons],
dnl OBSOLETE               AS_HELP_STRING([--disable-buttons],
dnl OBSOLETE                              [Disable buttons on the side of the window]))
dnl OBSOLETE AC_DEFINE([BUTTONS], 1, [Define to enable buttons on the side of the window.])

dnl OBSOLETE AC_ARG_ENABLE([statusline],
dnl OBSOLETE               AS_HELP_STRING([--disable-statusline],
dnl OBSOLETE                              [Disable statusline at bottom of window]))
dnl OBSOLETE AC_DEFINE([STATUSLINE], 1, [Define to enable statusline at bottom of window.])

dnl Required, not optional.
dnl AC_ARG_ENABLE([t1lib],
dnl               AS_HELP_STRING([--disable-t1lib],
dnl                              [Do not use T1lib (direct rendering of Type1 fonts)]))
AC_DEFINE([T1LIB], 1, [Define to compile in t1lib.])

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

AC_ARG_ENABLE([magick],
              AS_HELP_STRING([--enable-magick],
                             [Use ImageMagick to load/render image files.
                              EXPERIMENTAL - DO NOT USE!]))

AC_PROG_CXX

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
AC_TYPE_SIZE_T

dnl XDVI checks by Paul Vojta
XDVI_FUNC_SETSID_IN_VFORK
XDVI_CC_CONCAT
XDVI_C_BITMAP_TYPE
XDVI_SYS_STREAMS
XDVI_SYS_SUNOS_4
XDVI_SYS_OLD_LINUX
XDVI_FUNC_POLL

dnl #######

dnl COMPILER_WARNINGS

dnl #######

dnl SU: for `--allow-multiple-definition' linker flag on Cygwin	
XDVI_LINKER_MULTIPLE_DEFNS

AC_CHECK_HEADERS([stdint.h inttypes.h])
AC_TYPE_UINT32_T
dnl PB: rewrite Autoconf 2.13 style AC_CHECK_TYPE([ptrdiff_t], [long])
AC_CHECK_TYPE([ptrdiff_t], [],
              [AC_DEFINE_UNQUOTED([ptrdiff_t], [long],
                                  [Define to `long' if <sys/types.h> does not define.])])

AC_CHECK_FUNCS([mkstemp memcpy strerror waitpid sigaction strchr setenv ulltostr \
                fchdir getcwd getpwnam getuid getpwuid iconv nl_langinfo \
                lstat ftruncate snprintf vsnprintf realpath])

dnl <sys/bsdtypes.h> is for ISC 4.0, to define fd_set in psgs.c.
dnl regex.h is needed for regexp support in DVI search.    
AC_CHECK_HEADERS([netdb.h sys/bsdtypes.h sys/param.h unistd.h sys/fcntl.h regex.h iconv.h langinfo.h])

dnl Check X11 headers.
xdvi_save_CPPFLAGS=$CPPFLAGS
CPPFLAGS="$CPPFLAGS $X_CFLAGS"
AC_CHECK_HEADERS([X11/Intrinsic.h X11/Xosdefs.h X11/xpm.h Xm/XpmP.h])
dnl For editres, we need to check both the header and define a linker option.
AC_CHECK_HEADERS([X11/Xmu/Editres.h X11/Xmu/Xmu.h],
                 [x_xmu_lib=-lXmu],
                 [x_xmu_lib=],
                 [#ifdef HAVE_X11_INTRINSIC_H
# include <X11/Intrinsic.h>
#endif])
AC_SUBST([x_xmu_lib])
CPPFLAGS=$xdvi_save_CPPFLAGS

dnl check for Xaw version
XDVI_OLD_XAW

dnl nl_langinfo checks
AC_CACHE_CHECK([if nl_langinfo(CODESET) is working],
               [xdvi_cv_nl_langinfo_codeset],
               [AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <langinfo.h>]],
                                                [[char* cs = nl_langinfo(CODESET);]])],
                               [xdvi_cv_nl_langinfo_codeset=yes],
                               [xdvi_cv_nl_langinfo_codeset=no])])
AS_IF([test "x$xdvi_cv_nl_langinfo_codeset" = xyes],
      [AC_DEFINE([HAVE_WORKING_NL_LANGINFO_CODESET], 1,
                 [Define if the CODESET argument to nl_langinfo works.])])

dnl #######

dnl First, check for libXp and libXext

dnl #######

dnl Check whether -lXpm can be used	
XDVI_FIND_XPM

AC_ARG_WITH([motif-include],
            AS_HELP_STRING([--with-motif-include=DIR],
                           [Specify the location of Motif include files]))
AC_ARG_WITH([motif-libdir],
            AS_HELP_STRING([--with-motif-libdir=DIR],
                           [Specify the location of Motif libraries]))

AS_CASE([$with_xdvi_x_toolkit],
        [xaw3d],  [prog_extension="xaw3d"; x_tool_libs="-lXaw3d"],
        [neXtaw], [prog_extension="nextaw"; x_tool_libs="-lneXtaw"],
        [xaw],    [prog_extension="xaw"; x_tool_libs="-lXaw"
                   AC_DEFINE([XAW], 1, [Define to use the Xaw toolkit.])],
        [motif],  [XDVI_FIND_MOTIF([none])],
                  [XDVI_FIND_MOTIF([Xaw])])
AC_SUBST([x_tool_libs])
final_exec_name="xdvi-$prog_extension"
program_transform_name="'s,xdvi-bin,$final_exec_name,'"
AC_SUBST([final_exec_name])

dnl #######

AC_ARG_WITH([iconv-libdir],
[  --with-iconv-libdir=DIR
                          Specify the location of iconv library (librecode.so)],
[if test XXX"$withval" != "XXX"; then
    iconv_libpath="-L$withval"
    iconv_libs="-lrecode"
else
    iconv_libpath=
    iconv_libs=	    
fi], [
dnl On non-Linux systems, -liconv is needed. Check for both iconv_open and libiconv_open.
iconv_libpath=
iconv_libs=	
AC_CHECK_LIB(iconv, iconv_open, iconv_libs=-liconv,
		    AC_CHECK_LIB(iconv, libiconv_open, iconv_libs=-liconv,iconv_libs=,)
		    ,)     
])
if test XXX"$iconv_libs" != "XXX"; then
   AC_DEFINE([HAVE_ICONV], 1, [Define if you have the iconv function.])
fi    	
AC_SUBST([iconv_libpath])
AC_SUBST([iconv_libs])

AC_ARG_WITH([iconv-include],
[  --with-iconv-include=DIR
                          Specify the location of iconv header (iconv.h)],
[if test XXX"$withval" != "XXX"; then
    iconv_includes="-I$withval"
fi],[
iconv_includes=
])
AC_SUBST([iconv_includes])

XDVI_ICONV_CHAR_PPTR_TYPE

dnl #######

AC_SEARCH_LIBS([pow], [m])

KPSE_KPATHSEA_FLAGS
KPSE_T1LIB_FLAGS

XDVI_ARG_STRING([default-dvips-path],
[  --with-default-dvips-path=CMD
                          Use CMD as default DVI to PS converter],
[DEFAULT_DVIPS_PATH], ["dvips"],
[Define to point to the default command to use for printing (optional).])

XDVI_ARG_STRING([default-ps2pdf-path],
[  --with-default-ps2pdf-path=CMD
                          Use CMD as default PS to PDF converter],
[DEFAULT_PS2PDF_PATH], ["ps2pdf"],
[Define to point to the default command to use for printing (optional).])

dnl #######

XDVIK_VERSION=xdvik_version
AC_SUBST([XDVIK_VERSION])

AC_CONFIG_SUBDIRS([squeeze])

AC_CONFIG_FILES([Makefile gui/Makefile])

AC_CONFIG_FILES([xdvi:xdvi-sh.in], [chmod +x xdvi])

dnl The subdirectory squeeze must be configured for the build system.
dnl Can not share the cache file with the subdirectory!
AC_CONFIG_COMMANDS_POST([AS_IF([test "x$cross_compiling" = xyes],
[cache_file=/dev/null
 ac_configure_args="$ac_configure_args --host='$build' \
 CC='$BUILDCC' CPPFLAGS='$BUILDCPPFLAGS'\
 CFLAGS='$BUILDCFLAGS' LDFLAGS='$BUILDLDFLAGS'"])])

AC_OUTPUT
