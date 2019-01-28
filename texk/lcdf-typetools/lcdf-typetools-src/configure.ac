dnl Process this file with autoconf to produce a configure script.
AC_INIT([lcdf-typetools], [2.108])
AC_CONFIG_SRCDIR([NEWS.md])
AM_INIT_AUTOMAKE
AC_CONFIG_HEADERS([autoconf.h])
AM_MAINTAINER_MODE

AC_PROG_MAKE_SET

AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_CXXCPP

AC_ARG_ENABLE([warnings],
  [AS_HELP_STRING([--enable-warnings], [compile with -W -Wall])],
  [if test "$enableval" = yes; then
    CFLAGS="$CFLAGS -W -Wall"; CXXFLAGS="$CXXFLAGS -W -Wall"
  fi])

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


dnl AN_MAKEVAR([AR], [AC_PROG_AR])
dnl AN_PROGRAM([ar], [AC_PROG_AR])
dnl AC_DEFUN([AC_PROG_AR], [AC_CHECK_TOOL(AR, ar, :)])
dnl AC_PROG_AR

AC_PROG_RANLIB


dnl Available from the GNU Autoconf Macro Archive at:
dnl http://www.gnu.org/software/ac-archive/htmldoc/ac_cxx_template_objs.html
dnl
AC_DEFUN([AC_CXX_TEMPLATE_OBJS],
[AC_CACHE_CHECK(where template objects are stored, ac_cv_cxx_templobjs,
 [ ac_cv_cxx_templobjs='unknown'
   if test "$GXX" = yes; then
     ac_cv_cxx_templobjs='nowhere'
   else
     case $CXX in
       CC|*/CC)
        cat > conftest.cc <<EOF
template<class T> class A { public : A () {} };
template<class T> void f (const A<T>&) {}
main()
{ A<double> d;
  A<int> i;
  f (d);
  f (i);
  return 0;
}
EOF
        if test "$ac_cv_cxx_templobjs" = 'unknown' ; then
          if test -d Templates.DB ; then
            rm -fr Templates.DB
          fi
          if $CXX $CXXFLAGS -ptr. -c conftest.cc 1> /dev/null 2>&1; then
            if test -d Templates.DB ; then
#             this should be Sun CC <= 4.2
              CXXFLAGS="$CXXFLAGS -ptr."
              if test x"$LIBTOOL" = x ; then
                ac_cv_cxx_templobjs='Templates.DB/*.o'
              else
                ac_cv_cxx_templobjs='Templates.DB/*.lo'
              fi
              rm -fr Templates.DB
            fi
          fi
        fi
        if test "$ac_cv_cxx_templobjs" = 'unknown' ; then
          if test -d SunWS_cache ; then
            rm -fr SunWS_cache
          fi
          if $CXX $CXXFLAGS -c conftest.cc 1> /dev/null 2>&1; then
            if test -d SunWS_cache ; then
#             this should be Sun WorkShop C++ compiler 5.x
#             or Sun Forte C++ compiler >= 6.x
              if test x"$LIBTOOL" = x ; then
                ac_cv_cxx_templobjs='SunWS_cache/*/*.o'
              else
                ac_cv_cxx_templobjs='SunWS_cache/*/*.lo'
              fi
              rm -fr SunWS_cache
            fi
          fi
        fi
        rm -f conftest* ;;
     esac
   fi
   case "x$ac_cv_cxx_templobjs" in
     xunknown|xnowhere)
     TEMPLATE_OBJS="" ;;
     *)
     TEMPLATE_OBJS="$ac_cv_cxx_templobjs" ;;
   esac])
   AC_SUBST(TEMPLATE_OBJS)])

AC_CXX_TEMPLATE_OBJS


dnl
dnl standard headers
dnl (Though obsolete, AC_HEADER_STC changes the standard includes.)
dnl

AC_LANG_C
AC_HEADER_STDC
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
AC_CHECK_HEADERS([byteorder.h netinet/in.h sys/param.h])
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
    AC_MSG_ERROR([
=========================================

Cannot find a definition for ntohs and/or ntohl!

=========================================])
fi
AC_LANG_C
AC_SEARCH_LIBS([ntohs], [-lnet -lwinsock32])


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

m4_include([m4/lcdf-typetools.m4])

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

AC_ARG_WITH([kpathsea],
  [AS_HELP_STRING([--with-kpathsea=PREFIX], [Kpathsea is installed (under PREFIX)])],
  [kpathsea=$withval], [kpathsea=])

KPATHSEA_INCLUDES=
KPATHSEA_LIBS=
KPATHSEA_DEPEND=

if test "x$kpathsea" != xno; then
    SAVE_CPPFLAGS="$CPPFLAGS"; SAVE_LDFLAGS="$LDFLAGS"
    if test "x$kpathsea" != x -a "x$kpathsea" != xyes; then
	KPATHSEA_INCLUDES="-I$kpathsea/include"
	KPATHSEA_LIBS="-L$kpathsea/lib"
    else
	kpathsea=yes
    fi

    dnl check for kpathsea/kpathsea.h
    if true; then
	CPPFLAGS="$SAVE_CPPFLAGS $KPATHSEA_INCLUDES"
	AC_CHECK_HEADER([kpathsea/kpathsea.h], [kpse_header=yes], [kpse_header=no])
    fi

    dnl if not found and kpathsea prefix not set, check /usr/local
    if test "x$kpse_header" != xyes -a "x$kpathsea" = xyes -a -r /usr/local/include/kpathsea/tex-file.h; then
	KPATHSEA_INCLUDES="-I/usr/local/include"
	KPATHSEA_LIBS="-L/usr/local/lib"
	CPPFLAGS="$SAVE_CPPFLAGS $KPATHSEA_INCLUDES"
	AC_CHECK_HEADER([kpathsea/tex-file.h], [kpse_header=yes], [kpse_header=no])
    fi

    dnl check for -lkpathsea
    if true; then
	LDFLAGS="$SAVE_LDFLAGS $KPATHSEA_LIBS"
	AC_CHECK_LIB([kpathsea], [kpse_set_program_name], [kpse_library=yes], [kpse_library=no])
    fi

    dnl bail out if not found
    if test "x$kpse_header" != xyes -o "x$kpse_library" != xyes; then
	AC_MSG_ERROR([
=========================================

I can't find the kpathsea library and/or header files. Tell me where to look
using the --with-kpathsea=PREFIX option (header files should be under
PREFIX/include/kpathsea, and library under PREFIX/lib), or disable support
using --without-kpathsea.

=========================================])
    fi

    dnl OK, found
    AC_DEFINE(HAVE_KPATHSEA, 1, [Define if you want to use kpathsea.])
    if true; then
	KPATHSEA_LIBS="$KPATHSEA_LIBS -lkpathsea"
    fi

    dnl does it have opentype support?
    if false; then
	AC_DEFINE(HAVE_DECL_KPSE_OPENTYPE_FORMAT)
    else
	AC_CHECK_DECLS([kpse_opentype_format], [], [], [#include <kpathsea/kpathsea.h>])
    fi

    dnl does it have a separate format for encoding files?
    AC_CHECK_DECLS([kpse_enc_format], [], [], [#include <kpathsea/kpathsea.h>])

    CPPFLAGS="$SAVE_CPPFLAGS"; LDFLAGS="$SAVE_LDFLAGS"
fi

AM_CONDITIONAL([have_kpathsea], [test "x$with_kpathsea" != xno])
AC_SUBST([KPATHSEA_INCLUDES])
AC_SUBST([KPATHSEA_LIBS])
AC_SUBST([KPATHSEA_DEPEND])

AC_ARG_ENABLE([selfauto-set],
  [AS_HELP_STRING([--disable-selfauto-set], [disable setting SELFAUTO variables from kpsewhich])],
  [], [enable_selfauto_set=yes])

if test "x$enable_selfauto_set" = xyes -a "x$kpathsea" != xno; then
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

AC_ARG_ENABLE([glyphlistdir],
  [AS_HELP_STRING([--enable-glyphlistdir=DIR], [store glyphlist.txt files in DIR [pkgdatadir]])], [glyphlistdir="$enableval"], [glyphlistdir=NONE])
AC_ARG_ENABLE([encdir],
  [AS_HELP_STRING([--enable-encdir=DIR], [store encoding files in DIR [pkgdatadir]])], [encdir="$enableval"], [encdir=NONE])

test "x$glyphlistdir" = xNONE -o "x$glyphlistdir" = xno \
     -o "x$glyphlistdir" = x && glyphlistdir='${pkgdatadir}'
test "x$encdir" = xNONE -o "x$encdir" = xno \
     -o "x$encdir" = x && encdir='${pkgdatadir}'

AC_SUBST([glyphlistdir])
AC_SUBST([encdir])


dnl
dnl TeX Live build stub
dnl

KPATHSEA_RULE=
AC_SUBST([KPATHSEA_RULE])


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
    while echo "x$val" | grep '\$' >/dev/null 2>&1; do val=`eval echo "$val"`; done
    val=`echo "$val" | sed 's,//*,/,g'`
    eval "$1='$val'"
}

dnl Preset $prefix and $exec_prefix.
test "x$prefix" = xNONE && prefix=$ac_default_prefix
test "x$exec_prefix" = xNONE && exec_prefix='${prefix}'

pkgdatadir="${datadir}/$PACKAGE"
gdir="$glyphlistdir"
for i in data dataroot pkgdata g; do shell_expand ${i}dir; done
AC_DEFINE_UNQUOTED([GLYPHLISTDIR], ["$gdir"], [Directory for glyphlist.txt files.])


dnl
dnl Output
dnl

AC_OUTPUT(Makefile liblcdf/Makefile libefont/Makefile cfftot1/Makefile mmafm/Makefile mmpfb/Makefile otfinfo/Makefile otftotfm/Makefile t1dotlessj/Makefile t1lint/Makefile t1rawafm/Makefile t1reencode/Makefile t1testpage/Makefile ttftotype42/Makefile)
