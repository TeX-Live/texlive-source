dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009-2014 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
dnl *********************************************************************
dnl
dnl  Adapted for TeX Live from lcdf-typetools-src/configure.ac
dnl  Copyright (c) 1997-2011 Eddie Kohler
dnl
dnl *********************************************************************
dnl
m4_include([version.ac])[] dnl define lcdf_typetools_version
AC_INIT([LCDF typetools (TeX Live)], lcdf_typetools_version, [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([lcdf-typetools-src/NEWS.md])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIRS([../../m4 m4])

dnl Common code for all programs using libkpathsea.
KPSE_COMMON([lcdf-typetools])

AC_SUBST([LCDF_TYPETOOLS_VERSION], lcdf_typetools_version)

AC_CONFIG_HEADERS([autoconf.h])

AC_PROG_CXX
AC_PROG_CXXCPP

CC="$CC $WARNING_CFLAGS"
CXX="$CXX $WARNING_CXXFLAGS"

AC_ARG_ENABLE([precondition-checking],
  [AS_HELP_STRING([--enable-precondition-checking], [include precondition checking assertions])],
  [if test "$enableval" = yes; then
    AC_DEFINE([HAVE_PRECONDITION_CHECKING], [1], [Define to include precondition checking assertions.])
  fi])

AC_ARG_ENABLE([adobe-code],
  [AS_HELP_STRING([--disable-adobe-code], [do not include Adobe code])],
  [], [enable_adobe_code=yes])
if test "$enable_adobe_code" = yes; then
   AC_DEFINE([HAVE_ADOBE_CODE], [1], [Define to incldue Adobe code in output fonts.])
fi

AC_CXX_TEMPLATE_OBJS

KPSE_CXX_HACK


dnl
dnl directory searching
dnl

AC_LANG_C
AC_HEADER_DIRENT
AC_CHECK_HEADERS([fcntl.h unistd.h sys/time.h sys/wait.h])


dnl
dnl <new> and/or <new.h>
dnl

AC_LANG_CPLUSPLUS
AC_CACHE_CHECK([whether <new> works], ac_cv_good_new_hdr,
[AC_TRY_LINK([#include <new>], [
  int a;
  int *b = new(&a) int;
  return 0;
], ac_cv_good_new_hdr=yes, ac_cv_good_new_hdr=no)])
if test "$ac_cv_good_new_hdr" = yes; then
   AC_DEFINE([HAVE_NEW_HDR], [1], [Define if <new> exists and works.])
fi
AC_CHECK_HEADERS([new.h])


dnl
dnl endianness
dnl

AC_DEFINE([WORDS_BIGENDIAN_SET], [1], [Define if WORDS_BIGENDIAN has been set.])
AC_C_BIGENDIAN()


dnl
dnl functions such as strerror, working strtod, working strnlen, strtoul,
dnl time headers such as unistd.h
dnl

AC_LANG_C
need_fixlibc=0
AC_CHECK_FUNCS([strerror], [], [need_fixlibc=1])

AC_CACHE_CHECK([for broken strtod], [ac_cv_broken_strtod],
[AC_TRY_RUN([#include <stdio.h>
#include <stdlib.h>
int main(int c, char **v) {
  char s[] = "12 ";
  char *endp;
  double d = strtod(s, &endp);
  return (s + 2) == endp;
}], [ac_cv_broken_strtod=yes], [ac_cv_broken_strtod=no],
[ac_cv_broken_strtod=no])])
if test "x$ac_cv_broken_strtod" = xyes; then
    need_fixlibc=1
    AC_DEFINE([HAVE_BROKEN_STRTOD], [1], [Define if strtod is broken.])
fi

AC_LANG_CPLUSPLUS
AC_CHECK_DECLS([strnlen], [], [], [#include <string.h>])
AC_CHECK_FUNCS([strnlen], [have_strnlen=1], [need_fixlibc=1])
if test "x$have_strnlen" = x1; then
    AC_CACHE_CHECK([for broken strnlen], [ac_cv_broken_strnlen],
    [AC_TRY_RUN([#include <string.h>
#include <stdio.h>
int main(int c, char **v) {
  char s[] = "01234567891";
  return strnlen(s, 10) == 10;
}], [ac_cv_broken_strnlen=yes], [ac_cv_broken_strnlen=no],
[ac_cv_broken_strnlen=no])])
    if test "x$ac_cv_broken_strnlen" = xyes; then
	need_fixlibc=1
	AC_DEFINE([HAVE_BROKEN_STRNLEN], [1], [Define if strnlen is broken.])
    fi
fi
AC_LANG_C

AC_CHECK_FUNCS([ctime ftruncate mkstemp sigaction strdup strtoul vsnprintf waitpid])
AC_CHECK_FUNC([floor], [], [AC_CHECK_LIB([m], [floor])])
AC_CHECK_FUNC([fabs], [], [AC_CHECK_LIB([m], [fabs])])
AM_CONDITIONAL([FIXLIBC], [test x$need_fixlibc = x1])


dnl
dnl integer types
dnl

AC_CHECK_HEADERS(inttypes.h, have_inttypes_h=yes, have_inttypes_h=no)
AC_CHECK_HEADERS(sys/types.h, have_sys_types_h=yes, have_sys_types_h=no)

if test $have_inttypes_h = no -a $have_sys_types_h = yes; then
    AC_CACHE_CHECK([for uintXX_t typedefs], ac_cv_uint_t,
    [AC_EGREP_HEADER(dnl
changequote(<<,>>)<<(^|[^a-zA-Z_0-9])uint32_t[^a-zA-Z_0-9]>>changequote([,]),
    sys/types.h, ac_cv_uint_t=yes, ac_cv_uint_t=no)])
fi
if test $have_inttypes_h = no -a $have_sys_types_h = yes -a "$ac_cv_uint_t" = no; then
    AC_CACHE_CHECK([for u_intXX_t typedefs], ac_cv_u_int_t,
    [AC_EGREP_HEADER(dnl
changequote(<<,>>)<<(^|[^a-zA-Z_0-9])u_int32_t[^a-zA-Z_0-9]>>changequote([,]),
    sys/types.h, ac_cv_u_int_t=yes, ac_cv_u_int_t=no)])
fi
if test $have_inttypes_h = yes -o "$ac_cv_uint_t" = yes; then
    :
elif test "$ac_cv_u_int_t" = yes; then
    AC_DEFINE(HAVE_U_INT_TYPES, 1, [Define if you have u_intXX_t types but not uintXX_t types.])
else
    AC_MSG_WARN([
=========================================

Neither uint32_t nor u_int32_t defined by <inttypes.h> or <sys/types.h>!
Assuming "short" has 16 bits and "int" has 32 bits.

=========================================])
    AC_DEFINE(HAVE_FAKE_INT_TYPES, 1, [Define if intXX_t types are not available.])
fi

AC_CHECK_TYPES(uintptr_t, [], [],
[#if HAVE_INTTYPES_H
# include <inttypes.h>
#endif
#if HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif
])

AC_CHECK_SIZEOF(void *)
AC_CHECK_SIZEOF(unsigned long)
AC_CHECK_SIZEOF(unsigned int)


dnl
dnl ntohs, ntohl (need them in C++ code)
dnl

AC_LANG_CPLUSPLUS
AC_CHECK_HEADERS(byteorder.h netinet/in.h sys/param.h)
AC_MSG_CHECKING([whether ntohs and ntohl are defined])
ac_ntoh_defined=no
AC_COMPILE_IFELSE(
	[AC_LANG_PROGRAM([[#if HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif
#if HAVE_BYTEORDER_H
# include <byteorder.h>
#elif HAVE_NETINET_IN_H
# include <netinet/in.h>
#elif HAVE_SYS_PARAM_H
# include <sys/param.h>
#endif
#ifdef WIN32
# ifdef __MSC_VER
#  pragma warning (disable: 4290)
# endif
# include <winsock2.h>
#endif
]], [[(void) ntohs(0x0020), (void) ntohl(0x03040020);]])],
	[AC_MSG_RESULT(yes)
ac_ntoh_defined=yes],
	[AC_MSG_RESULT(no)])
if test $ac_ntoh_defined = no; then
    AC_CHECK_HEADERS([arpa/inet.h], [have_arpa_inet_h=yes], [have_arpa_inet_h=no])
    if test $have_arpa_inet_h = yes; then
	AC_MSG_CHECKING([whether ntohs and ntohl are defined in <arpa/inet.h>])
	AC_COMPILE_IFELSE(
		[AC_LANG_PROGRAM([[#if HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif
#include <arpa/inet.h>
]], [[(void) ntohs(0x0020), (void) ntohl(0x03040020);]])],
		[AC_MSG_RESULT(yes)
AC_DEFINE(NEED_ARPA_INET_H, 1, [Define to 1 if you must include <arpa/inet.h> to get `ntohl'.])
ac_ntoh_defined=yes],
		[AC_MSG_RESULT(no)])
    fi
fi
if test $ac_ntoh_defined = no; then
    AC_MSG_WARN([
=========================================

Cannot find a definition for ntohs and/or ntohl!

=========================================])
  if test "x$enable_build" != xno; then
    AC_MSG_ERROR([exiting])
  fi
fi
AC_LANG_C
AC_SEARCH_LIBS([ntohs], [-lnet -lwinsock32])

KPSE_CHECK_WIN32
if test "x$kpse_cv_have_win32" != xno; then
  LIBS="$LIBS -lwsock32"
fi

dnl
dnl is va_list addressable?
dnl

AC_LANG_CPLUSPLUS
AC_CACHE_CHECK([for addressable va_list type],
    ac_cv_va_list_addr,
    [AC_TRY_COMPILE([#include <stdarg.h>
void f(va_list *) {
}
void g(va_list val) {
    f(&val);
}
void h(int a, ...) {
    va_list val;
    va_start(val, a);
    g(val);
    va_end(val);
}], [h(2, 3, 4);], ac_cv_va_list_addr=yes, ac_cv_va_list_addr=no)])
if test "x$ac_cv_va_list_addr" = xyes; then
    AC_DEFINE(HAVE_ADDRESSABLE_VA_LIST, 1, [Define if the va_list type is addressable.])
fi


dnl
dnl select programs to be compiled and automatically run
dnl

m4_include([ac/lcdf-typetools.ac])

SELECTED_SUBDIRS=
AC_FOREACH([Kpse_Prog], kpse_lcdf_typetools_progs,
           [test "x$enable_[]Kpse_Prog" = xno || SELECTED_SUBDIRS="$SELECTED_SUBDIRS Kpse_Prog"
])
AC_SUBST(SELECTED_SUBDIRS)

AC_FOREACH([Kpse_Opt], kpse_otftotfm_auto_opts,
           [AS_IF([test "x$enable_auto_]Kpse_Opt[" != xno],
                  [AC_DEFINE([HAVE_AUTO_]AS_TR_CPP(Kpse_Opt), 1,
                             [Define to run ]Kpse_Opt[ from otftotfm.])])
])


dnl
dnl kpathsea
dnl

dnl The configure option '--enable-texlive-build' is inherited
dnl and automatically enabled, when built as part of TeX Live.

AM_CONDITIONAL([texlive_build], [test "x$enable_texlive_build" = xyes])

AC_ARG_WITH([kpathsea],
            AS_HELP_STRING([--without-kpathsea],
                           [build otftotfm without kpathsea]))

KPSE_KPATHSEA_FLAGS

if test "x$enable_build" != xno || test -f config.force; then

KPSE_ADD_FLAGS([kpathsea])

if test "x$with_kpathsea" != xno; then
  AC_MSG_CHECKING([for kpathsea headers and library])
  AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <kpathsea/kpathsea.h>]],
                                  [[kpse_tfm_format;]])],
                 [AC_MSG_RESULT([yes])
                  AC_CHECK_DECLS([kpse_opentype_format], , ,
                                 [#include <kpathsea/kpathsea.h>])
                  AC_CHECK_DECLS([kpse_enc_format], , ,
                                 [#include <kpathsea/kpathsea.h>])],
                 [AC_MSG_RESULT([no])
                  with_kpathsea=no])

  if test "x$with_kpathsea" = xno; then
    AC_MSG_ERROR([
=========================================

I can't find the kpathsea library and/or header files. Tell me where to look
using the --with-kpathsea-libdir=DIR option (library under DIR) and
--with-kpathsea-includes=DIR option (header files under DIR/kpathsea), or
disable kpathsea support using --without-kpathsea.

=========================================])
  fi
  AC_DEFINE(HAVE_KPATHSEA, 1, [Define if you want to use kpathsea.])
fi

KPSE_RESTORE_FLAGS

echo timestamp >config.force
fi

AM_CONDITIONAL([have_kpathsea], [test "x$with_kpathsea" != xno])

AC_ARG_ENABLE([selfauto-set],
  AS_HELP_STRING([--disable-selfauto-set], [disable setting SELFAUTO variables from kpsewhich]),
  [], [enable_selfauto_set=yes])

if test "x$enable_selfauto_set" = xyes -a "x$kpathsea" != xno -a "x$enable_texlive_build" != xyes; then
    kpsewhich='kpsewhich'
    test "x$kpathsea" != xyes -a -x "$kpathsea/bin/kpsewhich" && kpsewhich="$kpathsea/bin/kpsewhich"
    SELFAUTOLOC="`$kpsewhich --expand-var='$SELFAUTOLOC' 2>/dev/null`"
    SELFAUTODIR="`$kpsewhich --expand-var='$SELFAUTODIR' 2>/dev/null`"
    SELFAUTOPARENT="`$kpsewhich --expand-var='$SELFAUTOPARENT' 2>/dev/null`"
    SELFAUTOGRANDPARENT="`$kpsewhich --expand-var='$SELFAUTOGRANDPARENT' 2>/dev/null`"
    if test -z "$SELFAUTODIR"; then
	AC_MSG_WARN([
=========================================

Could not extract SELFAUTO variables from $kpsewhich.
Either supply the correct PREFIX to --with-kpathsea, or supply
--disable-selfauto-loc.

=========================================])
    fi
    AC_DEFINE_UNQUOTED([SELFAUTOLOC], "$SELFAUTOLOC", [kpsewhich's $SELFAUTOLOC variable])
    AC_DEFINE_UNQUOTED([SELFAUTODIR], "$SELFAUTODIR", [kpsewhich's $SELFAUTODIR variable])
    AC_DEFINE_UNQUOTED([SELFAUTOPARENT], "$SELFAUTOPARENT", [kpsewhich's $SELFAUTOPARENT variable])
    if test -n "$SELFAUTOGRANDPARENT" -a "$SELFAUTOGRANDPARENT" != '$SELFAUTOGRANDPARENT'; then
	AC_DEFINE_UNQUOTED([SELFAUTOGRANDPARENT], "$SELFAUTOGRANDPARENT", [kpsewhich's $SELFAUTOGRANDPARENT variable])
    fi
fi


dnl
dnl glyphlist.txt and *.enc installation locations
dnl
glyphlistdir='${datarootdir}/texmf-dist/fonts/map/glyphlist'
encdir='${datarootdir}/texmf-dist/fonts/enc/dvips/base'
AC_SUBST([glyphlistdir])
AC_SUBST([encdir])


dnl
dnl verbatim portions of the header
dnl

AC_DEFINE(HAVE_PERMSTRING, 1, [Define if PermStrings are available.])
AC_DEFINE(MMAFM_RUN_MMPFB, 1, [Define to 0 if you don't want mmafm to run mmpfb when it needs to get an intermediate master conversion program.])
dnl PATHNAME_SEPARATOR?


dnl
dnl set path variables
dnl

shell_expand () {
  val=`eval echo '$'"$1"`
  while echo "x$val" | fgrep '$' >/dev/null 2>&1; do val=`eval echo "$val"`; done
  eval "$1='$val'"
}

dnl Preset $prefix and $exec_prefix.
test "x$prefix" = xNONE && prefix=$ac_default_prefix
test "x$exec_prefix" = xNONE && exec_prefix='${prefix}'

shell_expand datadir

if test "x$enable_native_texlive_build" = xyes; then
  gdir=/nonesuch
else
  gdir="$glyphlistdir"
  for i in data dataroot pkgdata g; do shell_expand ${i}dir; done
fi
AC_DEFINE_UNQUOTED([GLYPHLISTDIR], ["$gdir"], [Directory for glyphlist.txt files.])


dnl
dnl Output
dnl

AC_SUBST([LCDF_TYPETOOLS_TREE], [lcdf-typetools-src])

AC_CONFIG_FILES([Makefile lcdf-typetools-src/Makefile])
AC_FOREACH([Kpse_Prog], [liblcdf libefont] kpse_lcdf_typetools_progs,
           [AC_CONFIG_FILES([lcdf-typetools-src/]Kpse_Prog[/Makefile])])

AC_OUTPUT
