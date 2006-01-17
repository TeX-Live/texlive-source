*** seetexk/configure.orig	Tue Mar 17 12:52:16 1998
--- seetexk/configure	Thu Mar 26 14:37:20 1998
***************
*** 2305,2311 ****
  s%@srcdir@%$srcdir%g
  s%@top_srcdir@%$top_srcdir%g
  s%@INSTALL@%$INSTALL%g
! " | (eval "$ac_sed_cmds") > $ac_file
  fi; done
  rm -f conftest.s*
  
--- 2305,2311 ----
  s%@srcdir@%$srcdir%g
  s%@top_srcdir@%$top_srcdir%g
  s%@INSTALL@%$INSTALL%g
! " | (eval "$ac_sed_cmds") | sed -e '/^default_texsizes/s,:,;,' > $ac_file
  fi; done
  rm -f conftest.s*
  
