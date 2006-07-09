/* config-vc6.h. config.h replacement for MS Visual C++ 6.0 */
/* config.h.in.  Generated automatically from configure.in by autoheader.  */

/* Define if using alloca.c.  */
/* #undef C_ALLOCA */

/* Define if type char is unsigned and you are not using gcc.  */
#ifndef __CHAR_UNSIGNED__
/* #undef __CHAR_UNSIGNED__ */
#endif

/* Define to empty if the keyword does not work.  */
/* #undef const */

/* Define to one of _getb67, GETB67, getb67 for Cray-2 and Cray-YMP systems.
   This function is required for alloca.c support on those systems.  */
/* #undef CRAY_STACKSEG_END */

/* Define if you have alloca, as a function or macro.  */
#define HAVE_ALLOCA 1

/* Define if you have <alloca.h> and it should be used (not on Ultrix).  */
/* #undef HAVE_ALLOCA_H */

/* Define if you have the ANSI # stringizing operator in cpp. */
/* #undef HAVE_STRINGIZE */

/* Define to `unsigned' if <sys/types.h> doesn't define.  */
/* #undef size_t */

/* If using the C implementation of alloca, define if you know the
   direction of stack growth for your system; otherwise it will be
   automatically deduced at run-time.
 STACK_DIRECTION > 0 => grows toward higher addresses
 STACK_DIRECTION < 0 => grows toward lower addresses
 STACK_DIRECTION = 0 => direction of growth unknown
 */
/* #undef STACK_DIRECTION */

/* Define if you have the ANSI C header files.  */
/* #undef STDC_HEADERS */

/* A type name of a signed integral type of exactly 16 bits. Always defined.
 */
#define PTS_INT16_T short

/* A type name of a signed integral type of exactly 32 bits. Always defined.
 */
#define PTS_INT32_T int

/* A type name of a signed integral type of exactly 64 bits. Maybe left
 * 0-defined.
 */
#define PTS_INT64_T long long

/* A type name of a signed integral type of exactly 128 bits. Maybe left
 * 0-defined.
 */
#define PTS_INT128_T 0

/* A type name of a signed integral type which has space to hold a pointer.
 * Always defined.
 */
#define PTS_INTP_T int

/* Imp: document this
 */
#define PTS_INTP_OK 1

/* Imp: document this
 */
/* #undef PTS_mode_t */

/* Imp: document this
 */
/* #undef PTS_off_t */

/* Imp: document this
 */
/* #undef PTS_pid_t */

/* Imp: document this
 */
#define PTS_size_t unsigned int

/* Just a dummy, non-existent type.
 */
/* #undef PTS_size_tt */

/* Imp: document this
 */
/* #undef PTS_uid_t */

/* Imp: document this
 */
/* #undef PTS_gid_t */

/* Imp: document this
 */
/* #undef PTS_dev_t */

/* Does the C compiler support prototypes?
 */
#define HAVE_PROTOTYPES 1

/* Define to empty if the keyword does not work.
 */
/* #undef volatile */

/* Imp: document this
 */
#define HAVE_strcpy_in_string 1

/* Imp: document this
 */
/* #undef HAVE_strcpy_in_strings */

/* Imp: document these
 */
/* #undef HAVE_MEMCMPY_BUILTIN */
#define HAVE_getc_in_stdio 1
#define HAVE_fgetc_in_stdio 1
#define HAVE_putc_in_stdio 1
#define HAVE_fputc_in_stdio 1
#define HAVE_atoi_in_stdlib 1
/* #undef HAVE_malloc_in_malloc */
#define HAVE_malloc_in_stdlib 1
#define HAVE_memcpy_in_stringxs 1
#define HAVE_sprintf_in_stdio 1
#define HAVE_write_in_unistd 1
#define HAVE_system_in_stdlib 1
/* #undef HAVE_lstat_in_sys_stat */
#define HAVE_PTS_STDC 1
/* #undef HAVE_SWITCH_ENUM_BUG */
#define HAVE_ASCII_SYSTEM 1
#define PTS_CFG_P_TMPDIR 0

/* Is the file c_gcc.cpp required for gcc when linking C++ programs? */
#define HAVE_PTS_C_LGCC_CPP_REQUIRED 1
/* #undef HAVE_PTS_C_LGCC3_CPP_REQUIRED */

/** Does the C library have a working vsnprintf()? */
/* #undef HAVE_PTS_VSNPRINTF */
/* #undef HAVE_PTS_VSNPRINTF_OLD */
/* #undef HAVE_PTS_VSNPRINTF_C99 */

/** What shell does the C library system(3) function call? */
#define HAVE_PTS_SYSTEMF 1
#define HAVE_PTS_SYSTEMF_WIN32 1
/* #undef HAVE_PTS_SYSTEMF_UNIX */
/* #undef HAVE_PTS_SYSTEMF_OTHER */

/** popen("...","wb") and "rb" works binary */
/* #undef HAVE_PTS_POPEN_B */

/** popen("...","w") and "r" works binary */
/* #undef HAVE_PTS_POPEN_ */

/* The number of bytes in a __int64.  */
#define SIZEOF___INT64 0

/* The number of bytes in a bool.  */
#define SIZEOF_BOOL 1

/* The number of bytes in a char.  */
#define SIZEOF_CHAR 1

/* The number of bytes in a char *.  */
#define SIZEOF_CHAR_P 4

/* The number of bytes in a int.  */
#define SIZEOF_INT 4

/* The number of bytes in a long.  */
#define SIZEOF_LONG 4

/* The number of bytes in a long long.  */
#define SIZEOF_LONG_LONG 8

/* The number of bytes in a short.  */
#define SIZEOF_SHORT 2

/* The number of bytes in a very long.  */
#define SIZEOF_VERY_LONG 0

/* The number of bytes in a void *.  */
#define SIZEOF_VOID_P 4

/* Can have static const int VARNAME=1; inside a class { ... } ? */
/* #undef HAVE_STATIC_CONST */

/* sam2p-specific */
#define USE_BUILTIN_ZIP 1
#define USE_BUILTIN_FAXE 1
#define USE_BUILTIN_LZW 1
#define USE_IN_GIF 1
#define USE_OUT_GIF 1
