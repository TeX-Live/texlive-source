/* c-auto.h.  Generated automatically by configure.  */
/* c-auto.in.  Generated automatically from configure.in by autoheader 2.13.  */
/* acconfig.h -- used by autoheader when generating c-auto.in.

   If you're thinking of editing acconfig.h to fix a configuration
   problem, don't. Edit the c-auto.h file created by configure,
   instead.  Even better, fix configure to give the right answer.  */

/* Guard against double inclusion. */
#ifndef KPATHSEA_C_AUTO_H
#define KPATHSEA_C_AUTO_H

/* kpathsea: the version string. */
#define KPSEVERSION "kpathsea version 6.3.4"

/* kpathsea/configure.in tests for these functions with
   kb_AC_KLIBTOOL_REPLACE_FUNCS, and naturally Autoheader doesn't know
   about that macro.  Since the shared library stuff is all preliminary
   anyway, I decided not to change Autoheader, but rather to hack them
   in here.  */
#define HAVE_PUTENV 1
#define HAVE_STRCASECMP 1
#define HAVE_STRTOL 1
#define HAVE_STRSTR 1
#define HAVE_DECL_STRSTR 1

/* Define if the closedir function returns void instead of int.  */
/* #undef CLOSEDIR_VOID */

/* Define to empty if the keyword does not work.  */
/* #undef const */

/* Define if your struct stat has st_mtim.  */
/* #undef HAVE_ST_MTIM */

/* Define if you have the ANSI C header files.  */
#define STDC_HEADERS 1

/* Define if your compiler understands prototypes.  */
#define HAVE_PROTOTYPES 1

/* Define if getcwd if implemented using fork or vfork.  Let me know
   if you have to add this by hand because configure failed to detect
   it. */
/* #undef GETCWD_FORKS */

/* Define if you are using GNU libc or otherwise have global variables
   `program_invocation_name' and `program_invocation_short_name'.  */
/* #undef HAVE_PROGRAM_INVOCATION_NAME */

/* all: Define to enable running scripts when missing input files.  */
#define MAKE_TEX_MF_BY_DEFAULT 1
#define MAKE_TEX_PK_BY_DEFAULT 1
#define MAKE_TEX_TEX_BY_DEFAULT 0
#define MAKE_TEX_TFM_BY_DEFAULT 1
#define MAKE_TEX_FMT_BY_DEFAULT 1
#define MAKE_OMEGA_OFM_BY_DEFAULT 0
#define MAKE_OMEGA_OCP_BY_DEFAULT 0

/* Define if you have the bcmp function.  */
/* #define HAVE_BCMP 1 */

/* Define if you have the bcopy function.  */
/* #define HAVE_BCOPY 1 */

/* Define if you have the bzero function.  */
/* #define HAVE_BZERO 1 */

/* Define if you have the getcwd function.  */
#define HAVE_GETCWD 1

/* Define if you have the getwd function.  */
/* #define HAVE_GETWD 1 */

/* Define if you have the index function.  */
/* #define HAVE_INDEX 1 */

/* Define if you have the memcmp function.  */
#define HAVE_MEMCMP 1

/* Define if you have the memcpy function.  */
#define HAVE_MEMCPY 1

/* Define if you have the putenv function.  */
#define HAVE_PUTENV 1

/* Define if you have the rindex function.  */
/* #define HAVE_RINDEX 1 */

/* Define if you have the strcasecmp function.  */
#define HAVE_STRCASECMP 1

/* Define if you have the strchr function.  */
#define HAVE_STRCHR 1

/* Define if you have the strrchr function.  */
#define HAVE_STRRCHR 1

/* Define if you have the strstr function.  */
#define HAVE_STRSTR 1

/* Define if you have the strtol function.  */
#define HAVE_STRTOL 1

/* Define if you have the <assert.h> header file.  */
#define HAVE_ASSERT_H 1

/* Define if you have the <dirent.h> header file.  */
#define HAVE_DIRENT_H 1

/* Define if you have the <dlfcn.h> header file.  */
/* #undef HAVE_DLFCN_H */

/* Define if you have the <float.h> header file.  */
#define HAVE_FLOAT_H 1

/* Define if you have the <limits.h> header file.  */
#define HAVE_LIMITS_H 1

/* Define if you have the <memory.h> header file.  */
#define HAVE_MEMORY_H 1

/* Define if you have the <ndir.h> header file.  */
/* #undef HAVE_NDIR_H */

/* Define if you have the <pwd.h> header file.  */
/* #define HAVE_PWD_H 1 */

/* Define if you have the <stdlib.h> header file.  */
#define HAVE_STDLIB_H 1

/* Define if you have the <string.h> header file.  */
#define HAVE_STRING_H 1

/* Define if you have the <strings.h> header file.  */
/* #undef HAVE_STRINGS_H */

/* Define if you have the <sys/dir.h> header file.  */
/* #undef HAVE_SYS_DIR_H */

/* Define if you have the <sys/ndir.h> header file.  */
/* #undef HAVE_SYS_NDIR_H */

/* Define if you have the <sys/param.h> header file.  */
/* #define HAVE_SYS_PARAM_H 1 */

/* Define if you have the <unistd.h> header file.  */
/* #define HAVE_UNISTD_H 1 */

#define strcasecmp stricmp
/* Define if you have the m library (-lm).  */
/* #undef HAVE_LIBM */

#define HAVE_INTTYPES_H 1
#define HAVE_STDINT_H 1

#endif /* !KPATHSEA_C_AUTO_H */
