dnl Copyright (C) 2001-2013 Peter Selinger.
dnl This file is part of Potrace. It is free software and it is covered
dnl by the GNU General Public License. See the file COPYING for details.

dnl Process this file with autoconf to produce a configure script.

dnl ----------------------------------------------------------------------
dnl Package info
AC_INIT([potrace],[1.11],[selinger at users.sourceforge.net])
AC_CONFIG_SRCDIR(src/trace.c)
AM_INIT_AUTOMAKE
AC_CONFIG_HEADER(config.h)
AC_CONFIG_MACRO_DIR([m4])

DATE="February 2013"

dnl ----------------------------------------------------------------------
dnl The names of the installed executables are defined here, but
dnl should not normally be changed.

POTRACE=potrace
AC_DEFINE_UNQUOTED(POTRACE,"$POTRACE",Name of the potrace binary)

MKBITMAP=mkbitmap
AC_DEFINE_UNQUOTED(MKBITMAP,"$MKBITMAP",Name of the mkbitmap binary)

dnl ----------------------------------------------------------------------
dnl figure out compiler options

dnl remember user's CFLAGS, if any
iCFLAGS="$CFLAGS"

dnl Check for compiler
AC_PROG_CC

dnl If compiler is gcc, add our own default CFLAGS if no others specified
if test "$GCC" = "yes" && test "$iCFLAGS" = ""; then
  CFLAGS="$CFLAGS -Wall -ffloat-store"
fi

dnl Also add CADD to the CFLAGS at configure time or compile time
AC_SUBST(CADD)

AC_MSG_CHECKING(what compiler options to use)
AC_MSG_RESULT($CFLAGS $CADD)

dnl ----------------------------------------------------------------------
dnl check if --with-libpotrace was given - this must be done
dnl before LT_INIT so that we can avoid building shared libraries
dnl when they are not going to be installed.

AC_ARG_WITH(libpotrace,
 AS_HELP_STRING([--with-libpotrace],[install Potrace library and headers [default=no]]))

if test "$with_libpotrace" != yes; then
 enable_shared=no
fi

dnl ----------------------------------------------------------------------
dnl Figure out how to build libraries

AC_LIBTOOL_WIN32_DLL
LT_INIT

dnl ----------------------------------------------------------------------
dnl Figure out whether to install libraries (default: no)

AC_MSG_CHECKING(whether to install the Potrace library)
if test "$with_libpotrace" = yes; then
 AC_MSG_RESULT(yes)
 INSTLIBS="libpotrace.la"
 INSTHEADERS="potracelib.h"
else
 AC_MSG_RESULT(no)
fi
AC_SUBST(INSTLIBS)
AC_SUBST(INSTHEADERS)

dnl ----------------------------------------------------------------------
dnl check for compiler bugs. 

dnl The GCC compiler used to have a loop optimization bug that affected
dnl potrace.  This bug had been present since gcc-2.X and was still
dnl pending (as of gcc version 3.3.1). Since the presence of this bug
dnl cannot be reliably tested, and I am not sure in which version it was
dnl fixed, we simply assume (conservatively) that all gcc compilers may
dnl have this bug. In future: add a test that checks the compiler
dnl version.

if test "$GCC" = "yes"; then 
  AC_MSG_CHECKING([whether gcc has bug number 12243])
  AC_DEFINE(HAVE_GCC_LOOP_BUG,, [Does the C compiler have gcc bug 12243?])
  AC_MSG_RESULT(maybe)
fi

dnl ----------------------------------------------------------------------
dnl check for features

AC_MSG_CHECKING(whether to use zlib)
AC_ARG_ENABLE(zlib,
 AS_HELP_STRING([--disable-zlib],[disable PostScript level 3 compression]))
if test "$enable_zlib" != no; then
  AC_DEFINE(HAVE_ZLIB,, Do we have the zlib library?)
  AC_MSG_RESULT(yes)
else
  AC_MSG_RESULT(no)
fi

dnl Set optional defaults
AC_MSG_CHECKING(whether to default to metric units)
AC_ARG_ENABLE(metric, 
 AS_HELP_STRING([--enable-metric],[use metric units (centimeters) as default]))
if test "$enable_metric" = yes; then
  AC_DEFINE(USE_METRIC,, Use metric units by default?)
  AC_MSG_RESULT(yes)
else
  AC_MSG_RESULT(no)
fi

AC_MSG_CHECKING(whether to default to a4 paper size)
AC_ARG_ENABLE(a4, 
 AS_HELP_STRING([--enable-a4],[use a4 as the default papersize]))
if test "$enable_a4" = yes; then
  AC_DEFINE(USE_A4,, Use a4 papersize by default?)
  AC_MSG_RESULT(yes)
else
  AC_MSG_RESULT(no)
fi

AC_MSG_CHECKING(whether to use the simplified progress bar)
AC_ARG_ENABLE(dumb-tty,
 AS_HELP_STRING([--enable-dumb-tty], [use simplified ASCII-only progress bar]))
dumb_tty="$enable_dumb_tty"
if test -z "$dumb_tty"; then
   AC_CANONICAL_HOST
   if test "$host_os" = "mingw32"; then
      dumb_tty="yes"
   fi
fi
if test "$dumb_tty" = "yes"; then
  AC_DEFINE(DUMB_TTY,, Use the simplified progress bar?)
  AC_MSG_RESULT(yes)
else
  AC_MSG_RESULT(no)
fi

dnl ----------------------------------------------------------------------
dnl Calculate value of "prefix", to aid guessing of include file and
dnl library locations.

USERPREFIX="$prefix"
if test "$USERPREFIX" = "NONE"; then
   USERPREFIX="$ac_default_prefix"
fi

dnl ----------------------------------------------------------------------
dnl Check for header files

if test "$enable_zlib" != no; then
  AC_CHECK_HEADER(zlib.h, true,
    [AC_MSG_WARN([zlib.h not found in standard location.
Trying again with -I$USERPREFIX/include])
    CPPFLAGS="$CPPFLAGS -I$USERPREFIX/include"
    dnl need to unset cached value to repeat the test
    unset ac_cv_header_zlib_h
    AC_CHECK_HEADER(zlib.h, true, 
      [AC_MSG_ERROR([cannot find zlib.h. 
Rerun ./configure with CPPFLAGS=-I<include dir> if you have the header
file in a non-standard directory, or with --disable-zlib to disable
support for PostScript level 3 compression.])
    ])
  ])
fi

dnl ----------------------------------------------------------------------
dnl Check for symbols
AC_CHECK_DECL(M_PI, ,
 AC_DEFINE(M_PI, 3.14159265358979323846, [Define if missing from math.h]),
 [#include <math.h>])

dnl ----------------------------------------------------------------------
dnl Check for libraries

if test "$enable_zlib" != no; then
  AC_CHECK_LIB(z, deflate, EXTRA_LIBS="$EXTRA_LIBS -lz", 
    [AC_MSG_WARN([libz library not found in standard location.
Trying again with -L$USERPREFIX/lib])
    LDFLAGS="$LDFLAGS -L$USERPREFIX/lib"
    dnl need to unset cached value to repeat the test
    unset ac_cv_lib_z_deflate
    AC_CHECK_LIB(z, deflate, EXTRA_LIBS="$EXTRA_LIBS -lz",
      [AC_MSG_ERROR([cannot find the libz library (-lz).
Rerun ./configure with LDFLAGS=-L<lib dir> if you have the library in
a non-standard directory, or with --disable-zlib to disable support
for PostScript level 3 compression.])
    ])
  ])
fi

AC_CHECK_LIB(m, floor, true, AC_MSG_ERROR([cannot find the m library (-lm)]))

dnl ----------------------------------------------------------------------
dnl Check for library functions.

dnl Check for getopt_long
AC_CHECK_FUNC(getopt_long, have_getopt_long=yes, have_getopt_long=no)
if test "$have_getopt_long" = "yes"; then
   dnl Check whether getopt_long reorders its arguments
   AC_MSG_CHECKING([whether getopt_long reorders its arguments])
   AC_RUN_IFELSE([AC_LANG_PROGRAM(
   [[ #include <getopt.h>
      static struct option longopts[] = {
        {"help", 0, 0, 'h'},
	{0, 0, 0, 0}
      };
   ]], 
   [[ int ac = 3;
      char *av[] = { "main", "file", "-h" };
      return 'h' == getopt_long(ac, av, "h", longopts, (int *)0) ? 0 : 1;
   ]])], 
   [AC_MSG_RESULT(yes)], 
   [AC_MSG_RESULT(no)
   have_getopt_long=no],
   [AC_MSG_RESULT(maybe (cross-compiling))
   have_getopt_long=no])
fi
if test "$have_getopt_long" != "yes"; then 
   EXTRA_OBJS="$EXTRA_OBJS getopt.o getopt1.o"
fi

AC_CHECK_FUNC(strcasecmp, ,
  [AC_CHECK_FUNC(stricmp, 
    [AC_DEFINE_UNQUOTED(strcasecmp, stricmp, [Replacement function for strcasecmp])],
    [AC_MSG_ERROR([cannot find a replacement for strcasecmp])]
  )]
)
AC_CHECK_FUNC(strncasecmp, ,
  [AC_CHECK_FUNC(strnicmp, 
    [AC_DEFINE_UNQUOTED(strncasecmp, strnicmp, [Replacement function for strncasecmp])],
    [AC_MSG_ERROR([cannot find a replacement for strncasecmp])]
  )]
)

dnl ----------------------------------------------------------------------
dnl Check whether we have i386 features
AC_MSG_CHECKING([for Intel 386])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[]], [[int x;
  asm("bsf %1,%0\njnz 0f\nmovl $32,%0\n0:":"=r"(x):"r"(x));
  return x; /* need this so that -O2 does not optimize the asm away */
]])],[AC_MSG_RESULT(yes) 
 AC_DEFINE(HAVE_I386,, Can we use Intel 386 optimizations?)
],[AC_MSG_RESULT(no)])	


dnl ----------------------------------------------------------------------
dnl Check whether compiler supports inlining

AC_C_INLINE

dnl ----------------------------------------------------------------------
dnl Set up substitutions of non-standard configuration parameters
AC_SUBST(DATE)
AC_SUBST(POTRACE)
AC_SUBST(MKBITMAP)
AC_SUBST(EXTRA_OBJS)
AC_SUBST(EXTRA_LIBS)
AC_SUBST(INCLUDES)
AC_SUBST(POTRACELIB_VERSION)

dnl ----------------------------------------------------------------------
AC_CONFIG_FILES([Makefile 
	   src/Makefile 
	   doc/Makefile
	   check/Makefile
	   doc/potrace.1
	   doc/mkbitmap.1
          ])
AC_OUTPUT
