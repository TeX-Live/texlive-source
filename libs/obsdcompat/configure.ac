# $Id: configure.ac,v 1.322.2.6 2006/02/08 11:11:06 dtucker Exp $
#
# Copyright (c) 1999-2004 Damien Miller
# Copyright (C) 2009-2012 Peter Breitenlohner <tex-live@tug.org>
#
# Permission to use, copy, modify, and distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#
# ---------------------------------------------------------------------------
#
# Modified (butchered) 2006 by Martin Schröder for pdfTeX
# Modified 2009-2012 by Peter Breitenlohner for TeX Live

AC_INIT([OpenBSD-Compat], [Portable], [tex-k@tug.org])
AC_REVISION($Revision: 1.322.2.6 $)
AC_CONFIG_SRCDIR([openbsd-compat.h])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

AM_INIT_AUTOMAKE([foreign])
AM_MAINTAINER_MODE

AM_CONDITIONAL([build], [test "x$enable_build" != xno])

AC_CONFIG_HEADERS([obsdcompat/config.h],
 [sed -e 's/^#define PACKAGE/#define OBSD_PACKAGE/' \
      -e 's/^#define VERSION/#define OBSD_VERSION/' \
    obsdcompat/config.h >obsdcompat/config.tmp && mv -f obsdcompat/config.tmp obsdcompat/config.h])
AC_PROG_CC
AC_CANONICAL_HOST
AC_C_BIGENDIAN

# Checks for programs.
AC_PROG_AWK
AC_PROG_RANLIB
AC_PROG_EGREP
AC_PATH_PROG(AR, ar)
AC_PATH_PROG(CAT, cat)
AC_PATH_PROG(SED, sed)
AC_PATH_PROG(TEST_MINUS_S_SH, bash)
AC_PATH_PROG(TEST_MINUS_S_SH, ksh)
AC_PATH_PROG(TEST_MINUS_S_SH, sh)
AC_PATH_PROG(SH, sh)
AC_SUBST(TEST_SHELL,sh)

# System features
AC_SYS_LARGEFILE

if test -z "$AR" ; then
	AC_MSG_ERROR([*** 'ar' missing, please install or fix your \$PATH ***])
fi

if test -z "$LD" ; then
	LD=$CC
fi
AC_SUBST(LD)

AC_C_INLINE

AC_CHECK_DECL(LLONG_MAX, have_llong_max=1, , [#include <limits.h>])

if test "$GCC" = "yes" || test "$GCC" = "egcs"; then
	CFLAGS="$CFLAGS -Wall -Wpointer-arith -Wuninitialized"
	GCC_VER=`$CC -v 2>&1 | $AWK '/gcc version /{print $3}'`
	case $GCC_VER in
		1.*) ;;
		2.8* | 2.9*) CFLAGS="$CFLAGS -Wsign-compare" ;;
		2.*) ;;
		3.*) CFLAGS="$CFLAGS -Wsign-compare" ;;
		4.*) CFLAGS="$CFLAGS -Wsign-compare -Wno-pointer-sign" ;;
		*) ;;
	esac

	if test -z "$have_llong_max"; then
		# now try LLONG_MIN with -std=gnu99, needed on some Linuxes
		saved_CFLAGS="$CFLAGS"
		CFLAGS="$CFLAGS -std=gnu99"
		AC_CHECK_DECL(LLONG_MIN,
		    [have_llong_max=1],
		    [CFLAGS="$saved_CFLAGS"],
		    [#include <limits.h>]
		)
	fi
fi

AC_ARG_WITH(rpath,
	[  --without-rpath         Disable auto-added -R linker paths],
	[
		if test "x$withval" = "xno" ; then
			need_dash_r=""
		fi
		if test "x$withval" = "xyes" ; then
			need_dash_r=1
		fi
	]
)

# Check for some target-specific stuff
case "$host" in
*-*-aix*)
	AC_MSG_CHECKING([how to specify blibpath for linker ($LD)])
	if (test -z "$blibpath"); then
		blibpath="/usr/lib:/lib"
	fi
	saved_LDFLAGS="$LDFLAGS"
	for tryflags in -blibpath: -Wl,-blibpath: -Wl,-rpath, ;do
		if (test -z "$blibflags"); then
			LDFLAGS="$saved_LDFLAGS $tryflags$blibpath"
			AC_LINK_IFELSE([AC_LANG_PROGRAM([[]], [[]])],[blibflags=$tryflags],[])
		fi
	done
	if (test -z "$blibflags"); then
		AC_MSG_RESULT(not found)
		AC_MSG_ERROR([*** must be able to specify blibpath on AIX - check config.log])
	else
		AC_MSG_RESULT($blibflags)
	fi
	LDFLAGS="$saved_LDFLAGS"
	;;
*-*-cygwin*)
	AC_DEFINE(HAVE_CYGWIN, 1, [Define if you are on Cygwin])
	;;
*-*-hpux*)
	# first we define all of the options common to all HP-UX releases
	CPPFLAGS="$CPPFLAGS -D_HPUX_SOURCE -D_XOPEN_SOURCE -D_XOPEN_SOURCE_EXTENDED=1"
	# next, we define all of the options specific to major releases
	case "$host" in
	*-*-hpux10*)
		if test -z "$GCC"; then
			CFLAGS="$CFLAGS -Ae"
		fi
		;;
	esac

	;;
*-*-irix5*)
	PATH="$PATH:/usr/etc"
	;;
*-*-irix6*)
	PATH="$PATH:/usr/etc"
	;;
*-*-linux*)
	no_dev_ptmx=1
	;;
mips-sony-bsd|mips-sony-newsos4)
	SONY=1
	;;
*-*-openbsd*)
	AC_DEFINE(HAVE_ATTRIBUTE__SENTINEL__, 1, [OpenBSD's gcc has sentinel])
	AC_DEFINE(HAVE_ATTRIBUTE__BOUNDED__, 1, [OpenBSD's gcc has bounded])
	;;
*-*-solaris*)
	if test "x$withval" != "xno" ; then
		need_dash_r=1
	fi
	;;
*-*-sunos4*)
	CPPFLAGS="$CPPFLAGS -DSUNOS4"
	;;
*-ncr-sysv*)
	LIBS="$LIBS -lc89"
	;;
*-sni-sysv*)
	# /usr/ucblib MUST NOT be searched on ReliantUNIX
	AC_CHECK_LIB(dl, dlsym, ,)
	;;
# UnixWare 1.x, UnixWare 2.x, and others based on code from Univel.
*-*-sysv4.2*)
	CFLAGS="$CFLAGS -Dva_list=_VA_LIST"
	;;
# UnixWare 7.x, OpenUNIX 8
*-*-sysv5*)
	;;
*-*-sysv*)
	;;
# SCO UNIX and OEM versions of SCO UNIX
*-*-sco3.2v4*)
	AC_MSG_ERROR("This Platform is no longer supported.")
	;;
# SCO OpenServer 5.x
*-*-sco3.2v5*)
	if test -z "$GCC"; then
		CFLAGS="$CFLAGS -belf"
	fi
	LIBS="$LIBS -lprot -lx -ltinfo -lm"
	TEST_SHELL=ksh
	;;
*-*-unicosmk*)
	LDFLAGS="$LDFLAGS"
	LIBS="$LIBS -lgen -lrsc -lshare -luex -lacm"
	;;
*-*-unicosmp*)
	LIBS="$LIBS -lgen -lacid -ldb"
	;;
*-*-unicos*)
	LDFLAGS="$LDFLAGS -Wl,-Dmsglevel=334:fatal"
	LIBS="$LIBS -lgen -lrsc -lshare -luex -lacm"
	;;
*-dec-osf*)
	;;

*-*-nto-qnx*)
	;;

*-*-ultrix*)
	;;

*-*-lynxos)
        ;;
esac

# Allow user to specify flags
AC_ARG_WITH(cflags,
	[  --with-cflags           Specify additional flags to pass to compiler],
	[
		if test -n "$withval"  &&  test "x$withval" != "xno"  &&  \
		    test "x${withval}" != "xyes"; then
			CFLAGS="$CFLAGS $withval"
		fi
	]
)
AC_ARG_WITH(cppflags,
	[  --with-cppflags         Specify additional flags to pass to preprocessor] ,
	[
		if test -n "$withval"  &&  test "x$withval" != "xno"  &&  \
		    test "x${withval}" != "xyes"; then
			CPPFLAGS="$CPPFLAGS $withval"
		fi
	]
)
AC_ARG_WITH(ldflags,
	[  --with-ldflags          Specify additional flags to pass to linker],
	[
		if test -n "$withval"  &&  test "x$withval" != "xno"  &&  \
		    test "x${withval}" != "xyes"; then
			LDFLAGS="$LDFLAGS $withval"
		fi
	]
)
AC_ARG_WITH(libs,
	[  --with-libs             Specify additional libraries to link with],
	[
		if test -n "$withval"  &&  test "x$withval" != "xno"  &&  \
		    test "x${withval}" != "xyes"; then
			LIBS="$LIBS $withval"
		fi
	]
)
AC_ARG_WITH(Werror,
	[  --with-Werror           Build main code with -Werror],
	[
		if test -n "$withval"  &&  test "x$withval" != "xno"; then
			werror_flags="-Werror"
			if test "x${withval}" != "xyes"; then
				werror_flags="$withval"
			fi
		fi
	]
)

AC_MSG_CHECKING(compiler and flags for sanity)
AC_RUN_IFELSE(
	[AC_LANG_SOURCE([
#include <stdio.h>
int main(){exit(0);}
	])],
	[	AC_MSG_RESULT(yes) ],
	[
		AC_MSG_RESULT(no)
		AC_MSG_ERROR([*** compiler cannot create working executables, check config.log ***])
	],
	[	AC_MSG_WARN([cross compiling: not checking compiler sanity]) ]
)

dnl Checks for header files.
AC_CHECK_HEADERS( \
	endian.h \
	features.h \
	floatingpoint.h \
	limits.h \
	stddef.h \
	stdint.h \
	string.h \
	strings.h \
	sys/stropts.h \
	sys/strtio.h \
	sys/sysmacros.h \
	util.h \
)

AC_FUNC_STRFTIME

dnl    Checks for library functions. Please keep in alphabetical order
AC_CHECK_FUNCS( \
	bcopy \
	strdup \
	strerror \
	strlcat \
	strlcpy \
	strtonum \
	strtoll \
	strtoul \
	sysconf \
)

dnl Make sure prototypes are defined for these before using them.
AC_CHECK_DECL(strsep,
	[AC_CHECK_FUNCS(strsep)],
	[],
	[
#ifdef HAVE_STRING_H
# include <string.h>
#endif
	])
	
# Check datatype sizes
AC_CHECK_SIZEOF(char, 1)
AC_CHECK_SIZEOF(short int, 2)
AC_CHECK_SIZEOF(int, 4)
AC_CHECK_SIZEOF(long int, 4)

# More checks for data types
AC_CACHE_CHECK([for u_int type], ac_cv_have_u_int, [
	AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[ #include <sys/types.h> ]], [[ u_int a; a = 1;]])],[ ac_cv_have_u_int="yes" ],[ ac_cv_have_u_int="no" 
	])
])
if test "x$ac_cv_have_u_int" = "xyes" ; then
	AC_DEFINE(HAVE_U_INT, 1, [define if you have u_int data type])
	have_u_int=1
fi

AC_CACHE_CHECK([for intXX_t types], ac_cv_have_intxx_t, [
	AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[ #include <sys/types.h> ]], [[ int8_t a; int16_t b; int32_t c; a = b = c = 1;]])],[ ac_cv_have_intxx_t="yes" ],[ ac_cv_have_intxx_t="no" 
	])
])
if test "x$ac_cv_have_intxx_t" = "xyes" ; then
	AC_DEFINE(HAVE_INTXX_T, 1, [define if you have intxx_t data type])
	have_intxx_t=1
fi

if (test -z "$have_intxx_t" && \
	   test "x$ac_cv_header_stdint_h" = "xyes")
then
    AC_MSG_CHECKING([for intXX_t types in stdint.h])
	AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[ #include <stdint.h> ]], [[ int8_t a; int16_t b; int32_t c; a = b = c = 1;]])],[
			AC_DEFINE(HAVE_INTXX_T)
			AC_MSG_RESULT(yes)
		],[ AC_MSG_RESULT(no) 
	])
fi

AC_CACHE_CHECK([for u_intXX_t types], ac_cv_have_u_intxx_t, [
	AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[ #include <sys/types.h> ]], [[ u_int8_t a; u_int16_t b; u_int32_t c; a = b = c = 1;]])],[ ac_cv_have_u_intxx_t="yes" ],[ ac_cv_have_u_intxx_t="no" 
	])
])
if test "x$ac_cv_have_u_intxx_t" = "xyes" ; then
	AC_DEFINE(HAVE_U_INTXX_T, 1, [define if you have u_intxx_t data type])
	have_u_intxx_t=1
fi

if test -z "$have_u_intxx_t" ; then
    AC_MSG_CHECKING([for u_intXX_t types in sys/socket.h])
	AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[ #include <sys/socket.h> ]], [[ u_int8_t a; u_int16_t b; u_int32_t c; a = b = c = 1;]])],[
			AC_DEFINE(HAVE_U_INTXX_T)
			AC_MSG_RESULT(yes)
		],[ AC_MSG_RESULT(no) 
	])
fi

if test -z "$have_u_intxx_t" ; then
	AC_CACHE_CHECK([for uintXX_t types], ac_cv_have_uintxx_t, [
		AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <sys/types.h>
			]], [[ uint8_t a; uint16_t b; uint32_t c; a = b = c = 1; ]])],[ ac_cv_have_uintxx_t="yes" ],[ ac_cv_have_uintxx_t="no" 
		])
	])
	if test "x$ac_cv_have_uintxx_t" = "xyes" ; then
		AC_DEFINE(HAVE_UINTXX_T, 1,
			[define if you have uintxx_t data type])
	fi
fi

if test -z "$have_uintxx_t" ; then
    AC_MSG_CHECKING([for uintXX_t types in stdint.h])
	AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[ #include <stdint.h> ]], [[ uint8_t a; uint16_t b; uint32_t c; a = b = c = 1;]])],[
			AC_DEFINE(HAVE_UINTXX_T)
			AC_MSG_RESULT(yes)
		],[ AC_MSG_RESULT(no) 
	])
fi

if (test -z "$have_u_intxx_t" || test -z "$have_intxx_t" && \
	   test "x$ac_cv_header_sys_bitypes_h" = "xyes")
then
	AC_MSG_CHECKING([for intXX_t and u_intXX_t types in sys/bitypes.h])
	AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <sys/bitypes.h>
		]], [[
			int8_t a; int16_t b; int32_t c;
			u_int8_t e; u_int16_t f; u_int32_t g;
			a = b = c = e = f = g = 1;
		]])],[
			AC_DEFINE(HAVE_U_INTXX_T)
			AC_DEFINE(HAVE_INTXX_T)
			AC_MSG_RESULT(yes)
		],[AC_MSG_RESULT(no)
	])
fi


AC_CACHE_CHECK([for u_char], ac_cv_have_u_char, [
	AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <sys/types.h>
		]], [[ u_char foo; foo = 125; ]])],[ ac_cv_have_u_char="yes" ],[ ac_cv_have_u_char="no" 
	])
])
if test "x$ac_cv_have_u_char" = "xyes" ; then
	AC_DEFINE(HAVE_U_CHAR, 1, [define if you have u_char data type])
fi

AC_CACHE_CHECK([for size_t], ac_cv_have_size_t, [
	AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <sys/types.h>
		]], [[ size_t foo; foo = 1235; ]])],[ ac_cv_have_size_t="yes" ],[ ac_cv_have_size_t="no" 
	])
])
if test "x$ac_cv_have_size_t" = "xyes" ; then
	AC_DEFINE(HAVE_SIZE_T, 1, [define if you have size_t data type])
fi

AC_CACHE_CHECK([for ssize_t], ac_cv_have_ssize_t, [
	AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <sys/types.h>
		]], [[ ssize_t foo; foo = 1235; ]])],[ ac_cv_have_ssize_t="yes" ],[ ac_cv_have_ssize_t="no" 
	])
])
if test "x$ac_cv_have_ssize_t" = "xyes" ; then
	AC_DEFINE(HAVE_SSIZE_T, 1, [define if you have ssize_t data type])
fi

dnl Adding -Werror to CFLAGS early prevents configure tests from running.
dnl Add now.
CFLAGS="$CFLAGS $werror_flags"

AC_EXEEXT
AC_CONFIG_FILES([Makefile])
AC_OUTPUT

echo "OpenBSD-Compat has been configured with the following options:"

echo ""

echo "              Host: ${host}"
echo "          Compiler: ${CC}"
echo "    Compiler flags: ${CFLAGS}"
echo "Preprocessor flags: ${CPPFLAGS}"
echo "      Linker flags: ${LDFLAGS}"
echo "         Libraries: ${LIBS}"

echo ""
