dnl @synopsis AX_ENABLE_BUILDDIR [(dirstring-or-command [,Makefile.mk [,-all]])]
dnl
dnl if the current configure was run within the srcdir then we move all
dnl configure-files into a subdir and let the configure steps continue
dnl there. We provide an option --disable-builddir to suppress the move
dnl into a separate builddir.
dnl
dnl Defaults:
dnl
dnl   $1 = $host (overridden with $HOST)
dnl   $2 = Makefile.mk
dnl   $3 = -all
dnl
dnl This macro must be called before AM_INIT_AUTOMAKE.
dnl
dnl it creates a default toplevel srcdir Makefile from the information
dnl found in the created toplevel builddir Makefile. It just copies the
dnl variables and rule-targets, each extended with a default
dnl rule-execution that recurses into the build directory of the
dnl current "HOST". You can override the auto-dection through
dnl `config.guess` and build-time of course, as in
dnl
dnl   make HOST=i386-mingw-cross
dnl
dnl which can of course set at configure time as well using
dnl
dnl   configure --host=i386-mingw-cross
dnl
dnl After the default has been created, additional rules can be
dnl appended that will not just recurse into the subdirectories and
dnl only ever exist in the srcdir toplevel makefile - these parts are
dnl read from the $2 = Makefile.mk file
dnl
dnl The automatic rules are usually scanning the toplevel Makefile for
dnl lines like '#### $host |$builddir' to recognize the place where to
dnl recurse into. Usually, the last one is the only one used. However,
dnl almost all targets have an additional "*-all" rule which makes the
dnl script to recurse into _all_ variants of the current HOST (!!)
dnl setting. The "-all" suffix can be overridden for the macro as well.
dnl
dnl a special rule is only given for things like "dist" that will copy
dnl the tarball from the builddir to the sourcedir (or $(PUB)) for
dnl reason of convenience.
dnl
dnl @category Misc
dnl @author Guido Draheim
dnl @version 2007-02-01
dnl @license GPLWithACException

AC_DEFUN([AX_ENABLE_BUILDDIR],[
AC_REQUIRE([AC_CANONICAL_HOST])[]dnl
AC_REQUIRE([AX_CONFIGURE_ARGS])[]dnl
AC_BEFORE([$0],[AM_INIT_AUTOMAKE])dnl
AS_VAR_PUSHDEF([SUB],[ax_enable_builddir])dnl
AS_VAR_PUSHDEF([AUX],[ax_enable_builddir_auxdir])dnl
AS_VAR_PUSHDEF([SED],[ax_enable_builddir_sed])dnl
SUB="."
AC_ARG_ENABLE([builddir], AC_HELP_STRING(
  [--disable-builddir],[disable automatic build in subdir of sources])
  ,[SUB="$enableval"], [SUB="yes"])
if test ".$ac_srcdir_defaulted" != ".no" ; then
if test ".$srcdir" = ".." ; then
  if test -f config.status ; then
    AC_MSG_NOTICE(toplevel srcdir already configured... skipping subdir build)
  else
    test ".$SUB" = "."  && SUB="."
    test ".$SUB" = ".no"  && SUB="."
    test ".$TARGET" = "." && TARGET="$target"
    test ".$SUB" = ".yes" && SUB="m4_ifval([$1], [$1],[$TARGET])"
    if test ".$SUB" != ".." ; then    # we know where to go and
      AS_MKDIR_P([$SUB])
      echo __.$SUB.__ > $SUB/conftest.tmp
      cd $SUB
      if grep __.$SUB.__ conftest.tmp >/dev/null 2>/dev/null ; then
        rm conftest.tmp
        AC_MSG_RESULT([continue configure in default builddir "./$SUB"])
      else
        AC_MSG_ERROR([could not change to default builddir "./$SUB"])
      fi
      srcdir=`echo "$SUB" |
              sed -e 's,^\./,,;s,[[^/]]$,&/,;s,[[^/]]*/,../,g;s,[[/]]$,,;'`
      # going to restart from subdirectory location
      test -f $srcdir/config.log   && mv $srcdir/config.log   .
      test -f $srcdir/confdefs.h   && mv $srcdir/confdefs.h   .
      test -f $srcdir/conftest.log && mv $srcdir/conftest.log .
      test -f $srcdir/$cache_file  && mv $srcdir/$cache_file  .
      AC_MSG_RESULT(....exec $SHELL $srcdir/[$]0 "--srcdir=$srcdir" "--enable-builddir=$SUB" ${1+"[$]@"})
      case "[$]0" in # restart
       [/\\]*) eval $SHELL "'[$]0'" "'--srcdir=$srcdir'" "'--enable-builddir=$SUB'" $ac_configure_args ;;
       *) eval $SHELL "'$srcdir/[$]0'" "'--srcdir=$srcdir'" "'--enable-builddir=$SUB'" $ac_configure_args ;;
      esac ; exit $?
    fi
  fi
fi fi
dnl ac_path_prog uses "set dummy" to override $@ which would defeat the "exec"
AC_PATH_PROG(SED,gsed sed, sed)
AUX="$ac_aux_dir" 
AUX=`echo "$AUX" | $SED -e "s|$ac_top_srcdir|.|"` 
test ".$AUX" = "." && AUX="$ac_aux_dir"
test ".$AUX" = "." && AUX="."
AS_VAR_POPDEF([SED])dnl
AS_VAR_POPDEF([AUX])dnl
AS_VAR_POPDEF([SUB])dnl
AC_CONFIG_COMMANDS([buildir],[dnl .............. config.status ..............
AS_VAR_PUSHDEF([SUB],[ax_enable_builddir])dnl
AS_VAR_PUSHDEF([TOP],[top_srcdir])dnl
AS_VAR_PUSHDEF([SRC],[ac_top_srcdir])dnl
AS_VAR_PUSHDEF([AUX],[ax_enable_builddir_auxdir])dnl
AS_VAR_PUSHDEF([SED],[ax_enable_builddir_sed])dnl
pushdef([END],[Makefile.mk])dnl
pushdef([_ALL],[ifelse([$3],,[-all],[$3])])dnl
  SRC="$ax_enable_builddir_srcdir"
  if test ".$SUB" = "." ; then
    if test -f "$TOP/Makefile" ; then
      AC_MSG_NOTICE([skipping TOP/Makefile - left untouched])
    else
      AC_MSG_NOTICE([skipping TOP/Makefile - not created])
    fi
  else
    if test -f "$SRC/Makefile" ; then
      a=`grep "^VERSION " "$SRC/Makefile"` ; b=`grep "^VERSION " Makefile`
      test "$a" != "$b" && rm "$SRC/Makefile"
    fi
    if test -f "$SRC/Makefile" ; then
	echo "$SRC/Makefile : $SRC/Makefile.in" > $tmp/conftemp.mk
	echo "	[]@ echo 'REMOVED,,,' >\$[]@" >> $tmp/conftemp.mk
      eval "${MAKE-make} -f $tmp/conftemp.mk 2>/dev/null >/dev/null"
      if grep '^REMOVED,,,' "$SRC/Makefile" >/dev/null
      then rm $SRC/Makefile ; fi
      cp $tmp/conftemp.mk $SRC/makefiles.mk~      ## DEBUGGING
    fi
    if test ! -f "$SRC/Makefile" ; then
      AC_MSG_NOTICE([create TOP/Makefile guessed from local Makefile])
      x='`' ; cat >$tmp/conftemp.sed <<_EOF
/^\$/n
x
/^\$/bS
x
/\\\\\$/{H;d;}
{H;s/.*//;x;}
bM
:S
x
/\\\\\$/{h;d;}
{h;s/.*//;x;}
:M
s/\\(\\n\\)	/\\1 /g
/^	/d
/^[[ 	]]*[[\\#]]/d
/^VPATH *=/d
s/^srcdir *=.*/srcdir = ./
s/^top_srcdir *=.*/top_srcdir = ./
/[[:=]]/!d
/^\\./d
s/:.*/:/
/:\$/s/ /  /g
/:\$/s/ \\([[a-z]][[a-z-]]*[[a-z]]\\) / \\1 \\1[]_ALL /g
/:\$/s/^\\([[a-z]][[a-z-]]*[[a-z]]\\)\\([[ :]]\\)/\\1 \\1[]_ALL\\2/
/:\$/s/  / /g
/^all all[]_ALL[[ :]]/i\\
all-configured : all[]_ALL
dnl dist-all exists... and would make for dist-all-all
/[]_ALL[]_ALL/d
/^.*[[=]]/!a\\
	@ HOST="\$(HOST)\" \\\\\\
	; test ".\$\$HOST" = "." && HOST=$x sh $AUX/config.guess $x \\\\\\
	; BUILD=$x grep "^#### \$\$HOST " Makefile | sed -e 's/.*|//' $x \\\\\\
	; use=$x basename "\$\@" _ALL $x; n=$x echo \$\$BUILD | wc -w $x \\\\\\
	; echo "MAKE \$\$HOST : \$\$n * \$\@"; if test "\$\$n" = "0" ; then : \\\\\\
	; BUILD=$x grep "^####.*|" Makefile |tail -1| sed -e 's/.*|//' $x ; fi \\\\\\
	; test ".\$\$BUILD" = "." && BUILD="." \\\\\\
	; test "\$\$use" = "\$\@" && BUILD=$x echo "\$\$BUILD" | tail -1 $x \\\\\\
	; for i in \$\$BUILD ; do test ".\$\$i" = "." && continue \\\\\\
	; (cd "\$\$i" && test ! -f configure && \$(MAKE) \$\$use) || exit; done
dnl special rule add-on: "dist" copies the tarball to $(PUB). (source tree)
/dist[]_ALL *:/a\\
	@ HOST="\$(HOST)\" \\\\\\
	; test ".\$\$HOST" = "." && HOST=$x sh $AUX/config.guess $x \\\\\\
	; BUILD=$x grep "^#### \$\$HOST " Makefile | sed -e 's/.*|//' $x \\\\\\
	; found=$x echo \$\$BUILD | wc -w $x \\\\\\
	; echo "MAKE \$\$HOST : \$\$found \$(PACKAGE)-\$(VERSION).tar.*" \\\\\\
	; if test "\$\$found" = "0" ; then : \\\\\\
	; BUILD=$x grep "^#### .*|" Makefile |tail -1| sed -e 's/.*|//' $x \\\\\\
	; fi ; for i in \$\$BUILD ; do test ".\$\$i" = "." && continue \\\\\\
	; for f in \$\$i/\$(PACKAGE)-\$(VERSION).tar.* \\\\\\
	; do test -f "\$\$f" && mv "\$\$f" \$(PUB). ; done ; break ; done
dnl special rule add-on: "distclean" removes all local builddirs completely
/distclean[]_ALL *:/a\\
	@ HOST="\$(HOST)\" \\\\\\
	; test ".\$\$HOST" = "." && HOST=$x sh $AUX/config.guess $x \\\\\\
	; BUILD=$x grep "^#### .*| *\\./" Makefile | sed -e 's/.*|//' $x \\\\\\
	; use=$x basename "\$\@" _ALL $x; n=$x echo \$\$BUILD | wc -w $x \\\\\\
	; echo "MAKE \$\$HOST : \$\$n * \$\@ (all local builds)" \\\\\\
	; test ".\$\$BUILD" = "." && BUILD="." \\\\\\
	; for i in \$\$BUILD ; do test ".\$\$i" = "." && continue \\\\\\
	; echo "# rm -r \$\$i"; done ; echo "# (sleep 3)" ; sleep 3 \\\\\\
	; for i in \$\$BUILD ; do test ".\$\$i" = "." && continue \\\\\\
	; echo "rm -r \$\$i"; (rm -r "\$\$i") ; done ; rm Makefile
_EOF
      cp "$tmp/conftemp.sed" "$SRC/makefile.sed~"            ## DEBUGGING
      $SED -f $tmp/conftemp.sed Makefile >$SRC/Makefile
      if test -f "$SRC/m4_ifval([$2],[$2],[END])" ; then
        AC_MSG_NOTICE([extend TOP/Makefile with TOP/m4_ifval([$2],[$2],[END])])
        cat $SRC/END >>$SRC/Makefile
      fi ; xxxx="####"
      echo "$xxxx CONFIGURATIONS FOR TOPLEVEL MAKEFILE: " >>$SRC/Makefile
      # sanity check
      if grep '^; echo "MAKE ' $SRC/Makefile >/dev/null ; then
        AC_MSG_NOTICE([buggy sed found - it deletes tab in "a" text parts])
        $SED -e '/^@ HOST=/s/^/	/' -e '/^; /s/^/	/' $SRC/Makefile \
          >$SRC/Makefile~
        (test -s $SRC/Makefile~ && mv $SRC/Makefile~ $SRC/Makefile) 2>/dev/null
      fi
    else
      xxxx="\\#\\#\\#\\#"
      # echo "/^$xxxx *$ax_enable_builddir_host /d" >$tmp/conftemp.sed
      echo "s!^$xxxx [[^|]]* | *$SUB *\$!$xxxx ...... $SUB!" >$tmp/conftemp.sed
      $SED -f "$tmp/conftemp.sed" "$SRC/Makefile" >$tmp/mkfile.tmp
        cp "$tmp/conftemp.sed" "$SRC/makefiles.sed~"         ## DEBUGGING
        cp "$tmp/mkfile.tmp"   "$SRC/makefiles.out~"         ## DEBUGGING
      if cmp -s "$SRC/Makefile" "$tmp/mkfile.tmp" 2>/dev/null ; then
        AC_MSG_NOTICE([keeping TOP/Makefile from earlier configure])
        rm "$tmp/mkfile.tmp"
      else
        AC_MSG_NOTICE([reusing TOP/Makefile from earlier configure])
        mv "$tmp/mkfile.tmp" "$SRC/Makefile"
      fi
    fi
    AC_MSG_NOTICE([build in $SUB (HOST=$ax_enable_builddir_host)])
    xxxx="####"
    echo "$xxxx" "$ax_enable_builddir_host" "|$SUB" >>$SRC/Makefile
  fi
popdef([END])dnl
AS_VAR_POPDEF([SED])dnl
AS_VAR_POPDEF([AUX])dnl
AS_VAR_POPDEF([SRC])dnl
AS_VAR_POPDEF([TOP])dnl
AS_VAR_POPDEF([SUB])dnl
],[dnl
ax_enable_builddir_srcdir="$srcdir"                    # $srcdir
ax_enable_builddir_host="$HOST"                        # $HOST / $host
ax_enable_builddir_version="$VERSION"                  # $VERSION
ax_enable_builddir_package="$PACKAGE"                  # $PACKAGE
ax_enable_builddir_auxdir="$ax_enable_builddir_auxdir" # $AUX
ax_enable_builddir_sed="$ax_enable_builddir_sed"       # $SED
ax_enable_builddir="$ax_enable_builddir"               # $SUB
])dnl
])
