*** configure.orig	Wed Mar 11 00:38:40 1998
--- configure	Tue Mar 17 11:38:18 1998
***************
*** 973,978 ****
--- 973,979 ----
  
  fi
    if test "${ac_cv_path_install+set}" = set; then
+     ac_cv_path_install=`echo.exe "$ac_cv_path_install"`
      INSTALL="$ac_cv_path_install"
    else
      # As a last resort, use the slow shell script.  We don't cache a
***************
*** 1986,1992 ****
  s%@srcdir@%$srcdir%g
  s%@top_srcdir@%$top_srcdir%g
  s%@INSTALL@%$INSTALL%g
! " | (eval "$ac_sed_cmds") > $ac_file
  fi; done
  rm -f conftest.s*
  
--- 1987,1993 ----
  s%@srcdir@%$srcdir%g
  s%@top_srcdir@%$top_srcdir%g
  s%@INSTALL@%$INSTALL%g
! " | (eval "$ac_sed_cmds") | sed -e '/^default_texsizes/s,:,;,' > $ac_file
  fi; done
  rm -f conftest.s*
  
