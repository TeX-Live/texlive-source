dnl ==========================================================================
dnl
dnl Cairo-specific macros
dnl

dnl ==========================================================================

dnl Usage:
dnl   CAIRO_BIGENDIAN
dnl
AC_DEFUN([CAIRO_BIGENDIAN],
[dnl
	case $host_os in
		darwin*)
	AH_VERBATIM([X_BYTE_ORDER],
[
/* Deal with multiple architecture compiles on Mac OS X */
#ifdef __APPLE_CC__
#ifdef __BIG_ENDIAN__
#define WORDS_BIGENDIAN 1
#define FLOAT_WORDS_BIGENDIAN 1
#else
#undef WORDS_BIGENDIAN
#undef FLOAT_WORDS_BIGENDIAN
#endif
#endif
])
		;;
		*) 
	AC_C_BIGENDIAN
	AX_C_FLOAT_WORDS_BIGENDIAN
		;;
	esac
])

