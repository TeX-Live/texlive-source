# configure.ac for XML2PMX

AC_INIT(xml2pmx, 2021-02-07, rdt@cs.queensu.ca)
AC_CONFIG_AUX_DIR(scripts)
AM_INIT_AUTOMAKE([-Wall foreign])
AC_CONFIG_SRCDIR(obx.h)
AC_CONFIG_FILES(Makefile)
AC_CONFIG_HEADERS(config.h)
AC_DEFINE(REVID, "xml2pmx", [Build id for runtime])

AC_CANONICAL_HOST
AC_PROG_CC
AC_C_CONST
AC_C_BIGENDIAN
AC_C_INLINE
AC_C_UNUSED
AC_C_USED
AC_C_NORETURN
AC_UNISTD_H

AC_DEFINE(PAGESIZE, 4096, [Notional page size for garbage collector])
AC_DEFINE(LOG_PAGESIZE, 12, [log2 of PAGESIZE])

# M64X32 allows Keiko programs (with 4-byte pointers) to run on machines
# with a 64-bit address space.
AC_CHECK_SIZEOF([int *])
if test $ac_cv_sizeof_int_p = 8; then
  AC_DEFINE(M64X32, 1, [Define for 64-bit runtime with 32-bit pointers])
fi

# Indexed jumps allow the interpreter to run faster, but it works
# without them.
# AC_C_INDEXED_JUMPS

# SEGMEM allows malloc to be used for all dynamic storage allocation
AC_DEFINE(SEGMEM, 1, [Define to simulate segmented memory])

AC_DEFINE(PRELOAD, 1, [Define to use preloaded image])
AC_DEFINE(MAGIC, "OBCX", [Magic number for executables])
AC_DEFINE(SIG, 0x00030290, [OBC version signature])
AC_DEFINE(DEBUG, 1, [Debug])

AC_OUTPUT
