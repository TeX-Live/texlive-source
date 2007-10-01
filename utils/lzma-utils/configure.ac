#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)

# [LZMA] instead of [LZMA utils] since I prefer to have lzma-version.tar.gz
# instead of lzma-utils-version.tar.gz.
AC_INIT([LZMA], [4.32.0beta5], [lasse.collin@tukaani.org])

AC_CONFIG_SRCDIR([src/sdk/7zip/ICoder.h])
AC_CONFIG_HEADER([config.h])

AC_SUBST(SDK_CXXFLAGS, "-DCOMPRESS_MF_BT -DCOMPRESS_MF_BT4 -DCOMPRESS_MF_HC")
AC_SUBST(SDK_CFLAGS, "-D_LZMA_IN_CB -D_LZMA_OUT_READ -D_LZMA_SYSTEM_SIZE_T -D_LZMA_PROB32")

# Debug or not:
AC_MSG_CHECKING([if debugging code should be compiled])
AC_ARG_ENABLE(debug,
	AC_HELP_STRING([--enable-debug], [Enable debugging code.]),
	enable_debug=yes,
	enable_debug=no)
if test "x$enable_debug" = xyes; then
	CFLAGS="-g $CFLAGS"
	CXXFLAGS="-g $CFLAGS"
	AC_MSG_RESULT([yes])
else
	AC_DEFINE(NDEBUG, 1, [Define to disable debugging code.])
	AC_MSG_RESULT([no])
fi

# There's no Fortran in LZMA utils, really.
F77=no

AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
# There is currently no workarounds in this package if some of
# these headers are missing:
AC_CHECK_HEADERS([fcntl.h getopt.h inttypes.h stddef.h stdlib.h string.h sys/time.h unistd.h utime.h], [], AC_MSG_ERROR([Required header file(s) are missing.]))
# Missing errno.h should be handled:
AC_CHECK_HEADERS([errno.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_HEADER_TIME
AC_C_CONST
AC_C_INLINE

AC_TYPE_MODE_T
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_CHECK_TYPE(int_fast8_t,,   [AC_DEFINE(int_fast8_t,   int8_t,   [Define to `int8_t' if <inttypes.h> or <stdint.h> does not define it.])])
AC_CHECK_TYPE(uint_fast8_t,,  [AC_DEFINE(uint_fast8_t,  uint8_t,  [Define to `uint8_t' if <inttypes.h> or <stdint.h> does not define it.])])
AC_CHECK_TYPE(int_fast16_t,,  [AC_DEFINE(int_fast16_t,  int16_t,  [Define to `int16_t' if <inttypes.h> or <stdint.h> does not define it.])])
AC_CHECK_TYPE(uint_fast16_t,, [AC_DEFINE(uint_fast16_t, uint16_t, [Define to `uint16_t' if <inttypes.h> or <stdint.h> does not define it.])])
AC_CHECK_TYPE(int_fast32_t,,  [AC_DEFINE(int_fast32_t,  int32_t,  [Define to `int32_t' if <inttypes.h> or <stdint.h> does not define it.])])
AC_CHECK_TYPE(uint_fast32_t,, [AC_DEFINE(uint_fast32_t, uint32_t, [Define to `uint32_t' if <inttypes.h> or <stdint.h> does not define it.])])
AC_CHECK_TYPE(int_fast64_t,,  [AC_DEFINE(int_fast64_t,  int64_t,  [Define to `int64_t' if <inttypes.h> or <stdint.h> does not define it.])])
AC_CHECK_TYPE(uint_fast64_t,, [AC_DEFINE(uint_fast64_t, uint64_t, [Define to `uint64_t' if <inttypes.h> or <stdint.h> does not define it.])])

AC_SYS_LARGEFILE

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_LSTAT
AC_CHECK_FUNCS([memmove strdup memset strerror strrchr strtol futimes])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 src/liblzmadec/Makefile
                 src/lzmadec/Makefile
                 src/lzmainfo/Makefile
                 src/lzma/Makefile
                 src/scripts/Makefile
                 src/sdk/7zip/Common/Makefile
                 src/sdk/7zip/Compress/LZ/Makefile
                 src/sdk/7zip/Compress/LZMA/Makefile
                 src/sdk/7zip/Compress/LZMA_C/Makefile
                 src/sdk/7zip/Compress/Makefile
                 src/sdk/7zip/Compress/RangeCoder/Makefile
                 src/sdk/7zip/Makefile
                 src/sdk/Common/Makefile
                 src/sdk/Makefile])
AC_OUTPUT

# This package should be compiled with large file support unless there's
# a very good reason to not do so.
if test "x$enable_largefile" = "xno"; then
  cat <<EOF

############################### WARNING ################################
  Large file support was disabled using the --disable-largefile
  configure option. This is NOT recommended unless you know what
  you are doing. In particular, you might need to compile apps using
  liblzmadec without large file support too. See README for details.
########################################################################

EOF
fi
