#                                               -*- m4 -*-
# Process this file with aclocal to produce a aclocal.m4 script.
dnl $Author$
dnl $Date$
dnl $Revision$

dnl Macros extending known languages to RC compiler
dnl Reference files : c.m4, lang.m4
dnl @synopsis AC_PROG_RC([COMPILER ...])

m4_define([rc_default_file],
[[STRINGTABLE
{
  100 "FooBar"
}]])

# AC_LANG(RC)
# -----------------------
m4_define([AC_LANG(RC)],
[ac_ext=rc
ac_cpp=''
ac_compile='$RC $RCFLAGS $CPPFLAGS $ax_cv_rc_compile >&AS_MESSAGE_LOG_FD'
ac_link=''
ac_compiler_gnu=
])

# *AC_LANG_*(RC)...
# -----------------------
AU_DEFUN([AC_LANG_RC], [AC_LANG(RC)])
m4_define([_AC_LANG_ABBREV(RC)], [rc])
m4_define([_AC_LANG_PREFIX(RC)], [RC])

# AC_LANG_SOURCE(RC)(BODY)
# -----------------------
m4_define([AC_LANG_SOURCE(RC)],
[/* confdefs.h.  */
_ACEOF
cat confdefs.h >>conftest.$ac_ext
cat >>conftest.$ac_ext <<_ACEOF
/* end confdefs.h.  */
$1])

# AC_LANG_*(RC)...
# -----------------------
m4_define([AC_LANG_PROGRAM(RC)],          [m4_error([$0 not supported])])
m4_define([AC_LANG_CALL(RC)],             [m4_error([$0 not supported])])
m4_define([AC_LANG_FUNC_LINK_TRY(RC)],    [m4_error([$0 not supported])])
m4_define([AC_LANG_BOOL_COMPILE_TRY(RC)], [m4_error([$0 not supported])])
m4_define([AC_LANG_BOOL_COMPILE_TRY(RC)], [m4_error([$0 not supported])])
m4_define([AC_LANG_INT_SAVE(RC)],         [m4_error([$0 not supported])])


# AC_PROG_RC([COMPILER ...])
# -----------------------
# Finds the RC compiler
AC_DEFUN([AC_PROG_RC],
[ AC_REQUIRE([AC_PROG_CC])
  AC_LANG_PUSH([RC])
  AX_PROG_ARGS([RC], [RC compiler])
  m4_ifval([$1],
	[AC_CHECK_TOOLS([RC], [$1])],
	[AC_CHECK_TOOL([RC], [windres])]
  )
  AX_PROG_MSG([$RC], [RC compiler])
  AX_LANG_CONFTEST( rc_default_file ,
    [ AC_MSG_CHECKING([whether the RC compiler works])
      AC_CACHE_VAL([ax_cv_rc_compile],
      [ ax_cv_rc_compile="-o conftest.$ac_objext conftest.$ac_ext"
	    _AX_COMPILE_IFELSE([], [ ax_cv_rc_compile='' ])
	  ])
  ])
  AX_MSG_YN_IFEMPTY([$ax_cv_rc_compile], [], [ AC_MSG_FAILURE([RC compiler "$RC" fails sanity check])])
  AC_LANG_POP
])
