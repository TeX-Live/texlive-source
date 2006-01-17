dnl Modifications for the latest version of Autoconf for kpathsea.
dnl These changes have all been sent back to the Autoconf maintainer via
dnl bug-gnu-utils@prep.ai.mit.edu.


dnl kb_AC_LIBTOOL_REPLACE_FUNCS(FUNCTION-NAME...)
AC_DEFUN(kb_AC_LIBTOOL_REPLACE_FUNCS,
[for ac_func in $1
do
AC_CHECK_FUNC($ac_func, , [LIBTOOL_LIBOBJS="$LIBTOOL_LIBOBJS ${ac_func}.lo"])
done
AC_SUBST(LIBTOOL_LIBOBJS)dnl
])


dnl Check if gcc asm for i386 needs external symbols with an underscore.
dnl Peter Breitenlohner, April 15, 1996.
AC_DEFUN([pb_AC_ASM_UNDERSCORE],
[AC_REQUIRE_CPP()dnl
AC_CACHE_CHECK(whether gcc asm needs underscore, pb_cv_asm_underscore,
[
# Older versions of GCC asm for i386 need an underscore prepended to
# external symbols. Figure out if this is so.
pb_cv_asm_underscore=yes
AC_TRY_LINK([
extern char val ;
extern void sub () ;
#if defined (__i386__) && defined (__GNUC__) 
asm("        .align 4\n"
".globl sub\n"
"sub:\n"
"        movb \$][1,val\n"
"        ret\n");
#else
void sub () { val = 1; }
#endif /* assembler */
char val ;
], [sub], pb_cv_asm_underscore=no)])
if test "x$pb_cv_asm_underscore" = xyes; then
  AC_DEFINE(ASM_NEEDS_UNDERSCORE)
fi
])

dnl Added /lib/... for A/UX.
dnl undefine([AC_PATH_X_DIRECT])dnl

dnl Changed make to ${MAKE-make}.
dnl undefine([AC_PATH_X_XMKMF])dnl

dnl Always more junk to check.
dnl undefine([AC_PATH_XTRA])dnl

dnl Added ac_include support.
dnl undefine([AC_OUTPUT_FILES])dnl


dnl From automake distribution, by Jim Meyering:
dnl Add --enable-maintainer-mode option to configure.

AC_DEFUN(AM_MAINTAINER_MODE,
[AC_MSG_CHECKING([whether to enable maintainer-specific portions of Makefiles])
  dnl maintainer-mode is disabled by default
  AC_ARG_ENABLE(maintainer-mode,
[  --enable-maintainer-mode enable make rules and dependencies not useful
                           (and sometimes confusing) to the casual installer],
      USE_MAINTAINER_MODE=$enableval,
      USE_MAINTAINER_MODE=no)
  AC_MSG_RESULT($USE_MAINTAINER_MODE)
  if test "x$USE_MAINTAINER_MODE" = xyes; then
    MAINT=
  else
    MAINT='#M#'
  fi
  AC_SUBST(MAINT)dnl
]
)

dnl The following three macros are copied from Thomas Dickey's autoconf
dnl patches at:
dnl 	http://dickey.his.com/autoconf/autoconf.html
dnl --Roozbeh Pournader

define(AC_DIVERSION_HELP, 3)dnl                user help-messages
define(ac_help_count, 0)dnl

dnl Send text to the user help-message list.  We will expand it as a here-
dnl document, so we'll split it to avoid too-long strings.
dnl AC_DIVERT_HELP(message)
define(AC_DIVERT_HELP,
[AC_DIVERT_PUSH(AC_DIVERSION_HELP)dnl
ifelse(ac_help_count,0,[--enable and --with options recognized:
])dnl
define([ac_help_count], builtin(eval, ac_help_count + 1))dnl
[$1]
ifelse(ac_help_count,13,[EOF
cat <<EOF
define([ac_help_count], 1)dnl
])dnl
AC_DIVERT_POP()dnl
])

AC_DEFUN(KPSE_STRUCT_ST_MTIM,
[AC_CACHE_CHECK([for st_mtim in struct stat], ac_cv_struct_st_mtim,
[AC_TRY_COMPILE([#include <sys/types.h>
#include <sys/stat.h>], [struct stat s; s.st_mtim;],
ac_cv_struct_st_mtim=yes, ac_cv_struct_st_mtim=no)])
if test $ac_cv_struct_st_mtim = yes; then
  AC_DEFINE(HAVE_ST_MTIM)
fi
])

# Find a program when cross-compiling, or use a default when not.
# $1 = variable which records the outcome
# $2 = program to look for when cross-compiling
# $3 = program to use when not cross-compiling
AC_DEFUN([KPSE_CROSS_PATH_PROG],
  [if test "$cross_compiling" = yes; then
AC_PATH_PROG([$1], [$2])
if test -z "${$1}"; then
  AC_MSG_ERROR([$2 was not found but is required when cross-compiling.
  Install $2 or set \$$1 to the full pathname.])
fi
else
  $1=$3
fi])

# Set BUILD$1 to $2 when cross-compiling, to $($1) if not.
# Example: KPSE_CROSS_BUILD_VAR([CC], [cc]) sets 'BUILDCC' to 'cc'
# if cross-compiling, and to '$(CC)' if not.
AC_DEFUN([KPSE_CROSS_BUILD_VAR],
  [if test "$cross_compiling" = yes; then
  if test -z "${BUILD$1}"; then
    BUILD$1='$2'
  fi
  AC_MSG_RESULT([setting \$BUILD$1 to ${BUILD$1}])
else
  if test -n "${BUILD$1}"; then
    AC_MSG_WARN([\$BUILD$1 set but not cross-compiling.])
  fi
  BUILD$1='$($1)'
fi
AC_SUBST(BUILD$1)
])

dnl # Preparing for autoconf-2.5x:
dnl #
dnl # As a first step towards autoconf-2.5x we define some new
dnl # autoconf-2.13 macros.
dnl #
dnl # 2005-02-20  Peter Breitenlohner  <peb@mppmu.mpg.de>

dnl # KPSE_CONFIG_FILES(FILE..., [COMMANDS], [INIT-CMDS])
dnl # ---------------------------------------------------
dnl # For now just an other name for AC_CONFIG_FILES, in future
dnl # (autoconf-2.5x) this will enable kpse_include substitution.
AC_DEFUN([KPSE_CONFIG_FILES], [AC_CONFIG_FILES([$1], [$2], [$3])])
