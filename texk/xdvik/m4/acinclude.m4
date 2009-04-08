dnl SU: the following is copied from gnome/compiler-flags.m4: turn on warnings for gcc
dnl
dnl COMPILER_WARNINGS
dnl Turn on many useful compiler warnings
dnl For now, only works on GCC
AC_DEFUN([COMPILER_WARNINGS],[
  AC_ARG_ENABLE(compiler-warnings, 
    [  --enable-compiler-warnings=[no/minimum/yes/maximum]
                          Turn on compiler warnings],,enable_compiler_warnings=minimum)

  AC_MSG_CHECKING(what warning flags to pass to the C compiler)
  warnCFLAGS=
  if test "x$GCC" != xyes; then
    enable_compiler_warnings=no
  fi

  if test "x$enable_compiler_warnings" != "xno"; then
    if test "x$GCC" = "xyes"; then
      case " $CFLAGS " in
      *[\ \	]-Wall[\ \	]*) ;;
      *) warnCFLAGS="-W -Wall -Wunused" ;;
      esac

      ## -W is not all that useful.  And it cannot be controlled
      ## with individual -Wno-xxx flags, unlike -Wall
      if test "x$enable_compiler_warnings" = "xyes"; then
        warnCFLAGS="$warnCFLAGS -pedantic -Wmissing-prototypes -Wmissing-declarations"
      elif test "x$enable_compiler_warnings" = "xmaximum"; then
      ## just turn on about everything:
      	warnCFLAGS="-Wall -Wunused -Wmissing-prototypes -Wmissing-declarations -Wimplicit -Wparentheses -Wreturn-type -Wswitch -Wtrigraphs -Wunused -Wshadow -Wpointer-arith -Wcast-qual -Wcast-align -Wwrite-strings"
      fi
    fi
  fi
  AC_MSG_RESULT($warnCFLAGS)

  ### FIXME: if we restrict setting the flags to this case only, they will get overridden
  ### somehwere else further up in the configure process.
  if test "x$cflags_set" != "xyes"; then
    XTRA_WARN_CFLAGS=$warnCFLAGS
    AC_SUBST(XTRA_WARN_CFLAGS)
#    CFLAGS="$CFLAGS $warnCFLAGS"
    cflags_set=yes
    AC_SUBST(cflags_set)
  fi
])

