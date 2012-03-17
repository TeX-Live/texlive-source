AC_PREREQ(2.54)

AC_INIT(silgraphite, 2.4, silgraphite-devel@lists.sourceforge.net)
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR(src/segment/GrEngine.cpp)
AC_CONFIG_AUX_DIR(config)

# setup automake
AM_INIT_AUTOMAKE(foreign)
AM_MAINTAINER_MODE

# System checks.
AC_CANONICAL_HOST
AC_C_BIGENDIAN

# Optional features
AC_ARG_ENABLE(tracing,
    AC_HELP_STRING([--disable-tracing],
        [build without support for trace Graphite logs]))
    test "x$enable_tracing" != xno && enable_tracing=yes

AC_ARG_ENABLE(debug,
    AC_HELP_STRING([--enable-debug],
        [build with debugging. (default=no)]))
AC_ARG_ENABLE(final,
    AC_HELP_STRING([--disable-final],
        [build without optimizations and no debugging.]))
    test "x$enable_final" != xno && enable_final=yes
AC_ARG_ENABLE(strict,
    AC_HELP_STRING([--enable-strict],
        [treat warnings as errors, check for as much as we can]))

AC_ARG_ENABLE(profile,
    AC_HELP_STRING([--enable-profile],
        [allow profiling (default=no)]))
AC_ARG_ENABLE(profilefn,
    AC_HELP_STRING([--enable-profilefn],
        [allow functioncheck profiling (default=no)]))

# Checks for programs.
AC_PROG_INSTALL
AC_PROG_LIBTOOL
AC_PROG_CC
AC_PROG_CXX
AC_LANG_CPLUSPLUS

AC_PATH_PROG(LSB_RELEASE, lsb_release, no)

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_CHECK_SIZEOF(wchar_t)
AC_SUBST(SIZEOF_WCHAR_T)

AC_CHECK_FUNCS(fabsf)

# Checks for library functions.

# Setup the build compiler flags and linker flags.
# Enable debug
# (note: the flags here are gcc-specific and may fail with other compilers)
build_flags=""
if test "$enable_debug" = yes; then
  build_flags="$build_flags -D_DEBUG -O0 -g -Wall -Wno-unknown-pragmas -Wparentheses"
elif test "$enable_strict" = yes; then
  build_flags="$build_flags -DNDEBUG -Wall -Wno-unknown-pragmas -Wparentheses -Wextra -Wendif-labels -Wshadow -Wno-ctor-dtor-privacy -Wno-non-virtual-dtor -Werror"
else
  build_flags="$build_flags -DNDEBUG"
fi

# Enable Graphite Trace logs - these are only used if a 
# LayoutEnvironment has a valid ostream pointer
if test "$enable_tracing" = yes; then
  build_flags="$build_flags -DTRACING"
fi

# Enable final
if test "$enable_final" = yes -a "$enable_debug" != yes; then
  build_flags="$build_flags -O3 -DNDEBUG"
fi
	
# Enable profile
if test "$enable_profile" = yes; then
  build_flags="$build_flags -pg "
fi
if test "$enable_profilefn" = yes; then
  build_flags="$build_flags -g -finstrument-functions "
  LIBS="$LIBS -g -finstrument-functions -lfnccheck "
fi

# Find distrib codename

AC_MSG_CHECKING(if already have REL_CODENAME)
if test -z "${REL_CODENAME}"; then
	AC_MSG_RESULT(no)
	AC_MSG_CHECKING(if have lsb_release)
	if test "${LSB_RELEASE}" = "no"; then
		AC_MSG_RESULT(no)
		REL_CODENAME="unstable"
	else
		AC_MSG_RESULT(yes)
		AC_MSG_CHECKING(if LSB_RELEASE is empty)
		if test -z ${LSB_RELEASE}; then
			AC_MSG_RESULT(yes)
			REL_CODENAME="unstable"
		else
			AC_MSG_RESULT(no)
			CODENAME=`${LSB_RELEASE} -c | sed -e 's/^\S*\s*//'`
			AC_MSG_CHECKING(if release is sid)
			if test ${CODENAME} = 'sid'; then
				AC_MSG_RESULT(yes)
				REL_CODENAME="unstable"
			else
				AC_MSG_RESULT(no, ${CODENAME})
				REL_CODENAME=${CODENAME}
			fi
		fi
	fi
else
	AC_MSG_RESULT(yes)
fi

AC_SUBST(REL_CODENAME)

CFLAGS="$CFLAGS $build_flags"
CXXFLAGS="$CXXFLAGS $build_flags"

AC_CONFIG_FILES(Makefile \
		silgraphite.pc:installed-top.pc.in:silgraphite.pc.in \
		silgraphite-uninstalled.pc:uninstalled-top.pc.in:silgraphite.pc.in \
		src/Makefile \
		test/Makefile \
		test/RegressionTest/Makefile \
        test/ProfileHarness/Makefile)

AC_OUTPUT
