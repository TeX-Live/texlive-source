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
        [next | nextaw | neXtaw], [val=neXtaw],
        [xaw3d], [val=xaw3d],
        ["" | x | xa | xaw], [val=xaw],
        [y | ye | yes], [val=yes],
        [m | mo | mot | moti | motif], [val=motif],
        [AC_MSG_WARN([unknown --with-xdvi-x-toolkit argument `$with_xdvi_x_toolkit'; using xaw.])
         val=xaw])
with_xdvi_x_toolkit=$val

AC_ARG_WITH([ps],
            AS_HELP_STRING([--with-ps=PS],
                           [Use PostScript (PS=no/yes/gs/dps/news, default gs)]))
AS_CASE([$with_ps],
        ["" | yes | gs], [PS_DEF=-DPS_GS],
        [no], [PS_DEF=],
        [dps], [PS_DEF=-DPS_DPS],
        [news], [PS_DEF=-DPS_NEWS],
        [AC_MSG_WARN([unknown--with-ps argument `$with_ps'; using gs.])
         PS_DEF=-DPS_GS])
AC_SUBST([PS_DEF])

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

AC_CONFIG_FILES([Makefile])

dnl The subdirectory squeeze must be configured for the build system.
dnl Can not share the cache file with the subdirectory!
AC_CONFIG_COMMANDS_POST([AS_IF([test "x$cross_compiling" = xyes],
[cache_file=/dev/null
 ac_configure_args="$ac_configure_args --host='$build' \
 CC='$BUILDCC' CPPFLAGS='$BUILDCPPFLAGS'\
 CFLAGS='$BUILDCFLAGS' LDFLAGS='$BUILDLDFLAGS'"])])

AC_OUTPUT
