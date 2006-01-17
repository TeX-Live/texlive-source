*** makeindexk/configure.orig	Tue Mar 17 12:52:06 1998
--- makeindexk/configure	Tue Sep 15 10:43:52 1998
***************
*** 2285,2291 ****
  s%@srcdir@%$srcdir%g
  s%@top_srcdir@%$top_srcdir%g
  s%@INSTALL@%$INSTALL%g
! " | (eval "$ac_sed_cmds") > $ac_file
  fi; done
  rm -f conftest.s*
  
--- 2285,2291 ----
  s%@srcdir@%$srcdir%g
  s%@top_srcdir@%$top_srcdir%g
  s%@INSTALL@%$INSTALL%g
! " | (eval "$ac_sed_cmds") | sed -e '/^default_texsizes/s,:,;,' > $ac_file
  fi; done
  rm -f conftest.s*
  
