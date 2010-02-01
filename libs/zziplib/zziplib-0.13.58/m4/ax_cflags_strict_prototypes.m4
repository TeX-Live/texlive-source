dnl @synopsis AX_CFLAGS_STRICT_PROTOTYPES [(shellvar [,default, [A/NA]]
dnl
dnl Try to find a compiler option that requires strict prototypes.
dnl
dnl The sanity check is done by looking at sys/signal.h which has a set
dnl of macro-definitions SIG_DFL and SIG_IGN that are cast to the local
dnl signal-handler type. If that signal-handler type is not fully
dnl qualified then the system headers are not seen as strictly
dnl prototype clean.
dnl
dnl For the GNU CC compiler it will be -fstrict-prototypes
dnl -Wstrict-prototypes The result is added to the shellvar being
dnl CFLAGS by default.
dnl
dnl DEFAULTS:
dnl
dnl  - $1 shell-variable-to-add-to : CFLAGS
dnl  - $2 add-value-if-not-found : nothing
dnl  - $3 action-if-found : add value to shellvariable
dnl  - $4 action-if-not-found : nothing
dnl
dnl @category C
dnl @author Guido Draheim <guidod@gmx.de>
dnl @version 2003-05-21
dnl @license GPLWithACException

AC_DEFUN([AX_CFLAGS_STRICT_PROTOTYPES],[dnl
AS_VAR_PUSHDEF([FLAGS],[CFLAGS])dnl
AS_VAR_PUSHDEF([VAR],[ac_cv_cflags_strict_prototypes])dnl
AC_CACHE_CHECK([m4_ifval($1,$1,FLAGS) for strict prototypes],
VAR,[VAR="no, unknown"
 AC_LANG_SAVE
 AC_LANG_C
 ac_save_[]FLAGS="$[]FLAGS"
for ac_arg dnl
in "-Wall     % -fstrict-prototypes -Wstrict-prototypes" dnl   GCC
   "-Wall     % -Wstrict-prototypes" dnl try to warn atleast
   #
do FLAGS="$ac_save_[]FLAGS "`echo $ac_arg | sed -e 's,%%.*,,' -e 's,%,,'`
   AC_TRY_COMPILE([],[return 0;],
   [VAR=`echo $ac_arg | sed -e 's,.*% *,,'` ; break])
done
case ".$VAR" in
   .|.no|.no,*) ;;
   *) # sanity check with signal() from sys/signal.h
    cp config.log config.tmp
    AC_TRY_COMPILE([#include <signal.h>],[
    if (signal (SIGINT, SIG_IGN) == SIG_DFL) return 1;
    if (signal (SIGINT, SIG_IGN) != SIG_DFL) return 2;],
    dnl the original did use test -n `$CC testprogram.c`
    [if test `diff config.log config.tmp | grep -i warning | wc -l` != 0
then if test `diff config.log config.tmp | grep -i warning | wc -l` != 1
then VAR="no, suppressed, signal.h," ; fi ; fi],
    [VAR="no, suppressed, signal.h"])
    rm config.tmp
  ;;
esac
 FLAGS="$ac_save_[]FLAGS"
 AC_LANG_RESTORE
])
case ".$VAR" in
     .ok|.ok,*) m4_ifvaln($3,$3) ;;
   .|.no|.no,*) m4_ifvaln($4,$4,[m4_ifval($2,[
        AC_RUN_LOG([: m4_ifval($1,$1,FLAGS)="$m4_ifval($1,$1,FLAGS) $2"])
                      m4_ifval($1,$1,FLAGS)="$m4_ifval($1,$1,FLAGS) $2"])]) ;;
   *) m4_ifvaln($3,$3,[
   if echo " $[]m4_ifval($1,$1,FLAGS) " | grep " $VAR " 2>&1 >/dev/null
   then AC_RUN_LOG([: m4_ifval($1,$1,FLAGS) does contain $VAR])
   else AC_RUN_LOG([: m4_ifval($1,$1,FLAGS)="$m4_ifval($1,$1,FLAGS) $VAR"])
                      m4_ifval($1,$1,FLAGS)="$m4_ifval($1,$1,FLAGS) $VAR"
   fi ]) ;;
esac
AS_VAR_POPDEF([VAR])dnl
AS_VAR_POPDEF([FLAGS])dnl
])

dnl the only difference - the LANG selection... and the default FLAGS

AC_DEFUN([AX_CXXFLAGS_STRICT_PROTOTYPES],[dnl
AS_VAR_PUSHDEF([FLAGS],[CXXFLAGS])dnl
AS_VAR_PUSHDEF([VAR],[ac_cv_cxxflags_strict_prototypes])dnl
AC_CACHE_CHECK([m4_ifval($1,$1,FLAGS) for strict prototypes],
VAR,[VAR="no, unknown"
 AC_LANG_SAVE
 AC_LANG_CXX
 ac_save_[]FLAGS="$[]FLAGS"
for ac_arg dnl
in "-Wall     % -fstrict-prototypes -Wstrict-prototypes" dnl   GCC
   "-Wall     % -Wstrict-prototypes" dnl try to warn atleast
   #
do FLAGS="$ac_save_[]FLAGS "`echo $ac_arg | sed -e 's,%%.*,,' -e 's,%,,'`
   AC_TRY_COMPILE([],[return 0;],
   [VAR=`echo $ac_arg | sed -e 's,.*% *,,'` ; break])
done
case ".$VAR" in
   .|.no|.no,*) ;;
   *) # sanity check with signal() from sys/signal.h
    cp config.log config.tmp
    AC_TRY_COMPILE([#include <signal.h>],[
    if (signal (SIGINT, SIG_IGN) == SIG_DFL) return 1;
    if (signal (SIGINT, SIG_IGN) != SIG_DFL) return 2;],
    dnl the original did use test -n `$CC testprogram.c`
    [if test `diff config.log config.tmp | grep -i warning | wc -l` != 0
then if test `diff config.log config.tmp | grep -i warning | wc -l` != 1
then VAR="no, suppressed, signal.h," ; fi ; fi],
    [VAR="no, suppressed, signal.h"])
    rm config.tmp
  ;;
esac
 FLAGS="$ac_save_[]FLAGS"
 AC_LANG_RESTORE
])
case ".$VAR" in
     .ok|.ok,*) m4_ifvaln($3,$3) ;;
   .|.no|.no,*) m4_ifvaln($4,$4,[m4_ifval($2,[
        AC_RUN_LOG([: m4_ifval($1,$1,FLAGS)="$m4_ifval($1,$1,FLAGS) $2"])
                      m4_ifval($1,$1,FLAGS)="$m4_ifval($1,$1,FLAGS) $2"])]) ;;
   *) m4_ifvaln($3,$3,[
   if echo " $[]m4_ifval($1,$1,FLAGS) " | grep " $VAR " 2>&1 >/dev/null
   then AC_RUN_LOG([: m4_ifval($1,$1,FLAGS) does contain $VAR])
   else AC_RUN_LOG([: m4_ifval($1,$1,FLAGS)="$m4_ifval($1,$1,FLAGS) $VAR"])
                      m4_ifval($1,$1,FLAGS)="$m4_ifval($1,$1,FLAGS) $VAR"
   fi ]) ;;
esac
AS_VAR_POPDEF([VAR])dnl
AS_VAR_POPDEF([FLAGS])dnl
])
