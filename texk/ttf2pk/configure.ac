dnl This file is part of the ttf2pk package

dnl Process this file with autoconf to produce a configure script.

dnl Some tests are omitted since we assume that you've built the
dnl FreeType library successfully.

AC_INIT
AC_CONFIG_SRCDIR([../../libs/freetype.h])

AC_CANONICAL_TARGET([])

AC_PROG_CC
AC_PROG_CPP

AC_ARG_WITH(kpathsea-include,
            [  --with-kpathsea-include=DIR
                          location of the kpathsea include dir],[
  if test x$withval = xyes; then
    AC_MSG_WARN(Usage is: --with-kpathsea-include=includedir)
  else
    if test x$withval = xno; then
      AC_MSG_WARN(Usage is: --with-kpathsea-include=includedir)
    else
      kpathsea_include=$withval
    fi
  fi
])


dnl Checks for libraries.
AC_ARG_WITH(kpathsea-lib,
            [  --with-kpathsea-lib=DIR location of the kpathsea lib dir],[
  if test x$withval = xyes; then
    AC_MSG_WARN(Usage is: --with-kpathsea-lib=libdir)
  else
    if test x$withval = xno; then
      AC_MSG_WARN(Usage is: --with-kpathsea-lib=libdir)
    else
      kpathsea_lib=$withval
    fi
  fi
])


if test -n "$kpathsea_include" -o -n "$kpathsea_lib"; then
  LIBS="$LIBS -L$kpathsea_lib"
  CPPFLAGS="$CPPFLAGS -I$kpathsea_include -I$srcdir"

  dnl the function kpse_set_program_name() is available since kpathsea 3.2
  AC_CHECK_LIB(kpathsea, kpse_set_program_name, , AC_DEFINE(OLD_KPATHSEA))

  AC_CHECK_LIB(kpathsea, kpse_init_prog, ,[
               AC_MSG_ERROR([Can't find kpathsea library! Use --with-kpathsea-lib option.])])

  dnl the header file kpathsea.h doesn't exist in kpathsea 2.6 and before
  AC_CHECK_HEADER(kpathsea/kpathsea.h, , AC_DEFINE(VERY_OLD_KPATHSEA))

  AC_CHECK_HEADER(kpathsea/c-auto.h, ,[
                  AC_MSG_ERROR([Can't find kpathsea include files! Use --with-kpathsea-include option.])])
else
  CPPFLAGS="$CPPFLAGS -I$srcdir"
fi


AC_CHECK_LIB(m, floor)
AC_CHECK_LIB(intl, gettext)

OLDLIBS=$LIBS
LIBS="$LIBS -L../../lib/.libs"
CPPFLAGS="-I$srcdir/../../lib $CPPFLAGS"
AC_CHECK_LIB(ttf, TT_Init_FreeType, LIBS="$LIBS -lttf",[
             AC_MSG_ERROR([Can't find ttf library! Compile FreeType first.])])
LIBS=$OLDLIBS

dnl get Compiler flags right.

if test "x$CC" = xgcc; then
	XX_CFLAGS="-Wall -pedantic"
else
	case "$host" in
  		alpha-dec-osf*)
        	XX_CFLAGS="-std1 -O2 -g3"
       		;;
  		*)
          	XX_CFLAGS=
       		;;
	esac
fi
AC_SUBST(XX_CFLAGS)

AC_CHECK_PROG(RM, rm, rm)
AC_CHECK_PROG(RMDIR, rmdir, rmdir)
AC_PROG_INSTALL

if test -z "$kpathsea_include" -o -z "$kpathsea_lib"; then
  AC_MSG_WARN([

    The binaries will be compiled without file search library support!
    For kpathsea support use the --with-kpathsea-lib and
    --with-kpathsea-include options.
              ])
fi

AC_CONFIG_FILES([Makefile MakeSub])
AC_OUTPUT

dnl end of configure.in
