dnl ### Process a string argument

dnl XDVI_ARG_STRING(PACKAGE, HELP-STRING, VARIABLE, DEFAULT_VALUE, DESCRIPTION)
AC_DEFUN([XDVI_ARG_STRING],
[AC_ARG_WITH([$1], [$2], [AC_DEFINE_UNQUOTED($3, ["$withval"], [$5])
], [AC_DEFINE([$3], [$4])
])])


