#                                               -*- m4 -*-
# Process this file with aclocal to produce a aclocal.m4 script.
dnl $Author$
dnl $Date$
dnl $Revision$
dnl
dnl Multiple functions extending AutoConf/m4, most functions are shared among
dnl several other m4 files. Think of it as a 'sugar'
dnl A frozen m4sugar.m4 can be problematic

# m4_error(MSG, [EXIT-STATUS])
# -----------------------
# Fatal the user.   
m4_define([m4_error],
[m4_errprintn(m4_location[: error: $1])dnl
m4_exit(m4_if([$2],, 1, [$2]))])

# m4_ifnval(COND, [IF-FALSE], [IF-TRUE])
# -------------------------------------
# Opposite of m4_ifval
m4_define([m4_ifnval], [m4_if([$1], [], [$2], [$3])])

# AX_PROG_LOG_VERSION
# -----------------------
# Logs --version information for current $ac_compiler
AC_DEFUN([AX_PROG_LOG_VERSION],
[ _AS_ECHO_LOG([checking for _AC_LANG compiler version])
  ac_compiler=`set X $ac_compile; echo $[2]`
  _AC_EVAL([$ac_compile --version >&AS_MESSAGE_LOG_FD])
  _AC_EVAL([$ac_compile -v >&AS_MESSAGE_LOG_FD])
  _AC_EVAL([$ac_compile -V >&AS_MESSAGE_LOG_FD])
])

# AX_PROG_ARGS(PREFIX, STRING NAME)
# -----------------------
AC_DEFUN([AX_PROG_ARGS],
[ m4_ifnval([$1], [m4_error([AX_PROG_ARGS(PREFIX, STRING NAME) is missing variable 'PREFIX'.])])
  m4_ifnval([$2], [m4_error([AX_PROG_ARGS(PREFIX, STRING NAME) is missing variable 'STRING NAME'.])])
  AC_ARG_VAR([$1], [$2 command])
  AC_ARG_VAR(m4_join([], [$1], [FLAGS]), [$2 flags])
  AC_SUBST(m4_join([], [AM_], [$1], [FLAGS]), [])
])

# _AX_COMPILE_IFELSE([SUCCESS], [FAILURE])
# -----------------------
AC_DEFUN([_AX_COMPILE_IFELSE],
[ _AC_COMPILE_IFELSE([], [$1], [$2])
])

# AX_MSG_YN_IFEMPTY(VARIABLE, [IF-EMPTY], [IF-NOT-EMPTY])
# -----------------------
AC_DEFUN([AX_MSG_YN_IFEMPTY],
[ if test -z "$1" ; then
    AC_MSG_RESULT([no])
	$3
  else
    AC_MSG_RESULT([yes])
	$2
  fi
])

# AX_PROG_MSG(VARIABLE, STRING NAME)
# -----------------------
# Check to see $VARIABLE was set, and log $VARIABLE --version
AC_DEFUN([AX_PROG_MSG],
[ m4_ifnval([$2], [m4_error([AX_PROG_MSG(VARIABLE, STRING NAME) is missing variable 'STRING NAME'.])])
  test -z "$1" && AC_MSG_FAILURE([no acceptable $2 found in \$PATH])
  AX_PROG_LOG_VERSION
])

# AX_LANG_CONFTEST(CONTENT, ACTIONS)
# -----------------------
AC_DEFUN([AX_LANG_CONFTEST],
[ AC_LANG_CONFTEST([[$1]])
  $2
  rm -f conftest.$ac_ext
])
