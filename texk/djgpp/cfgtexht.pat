*** tex4htk/configure.orig	Tue Mar 17 12:52:20 1998
--- tex4htk/configure	Wed Sep 16 19:06:00 1998
***************
*** 2210,2216 ****
  s%@srcdir@%$srcdir%g
  s%@top_srcdir@%$top_srcdir%g
  s%@INSTALL@%$INSTALL%g
! " | (eval "$ac_sed_cmds") > $ac_file
  fi; done
  rm -f conftest.s*
  
--- 2210,2216 ----
  s%@srcdir@%$srcdir%g
  s%@top_srcdir@%$top_srcdir%g
  s%@INSTALL@%$INSTALL%g
! " | (eval "$ac_sed_cmds") | sed -e '/^default_texsizes/s,:,;,' > $ac_file
  fi; done
  rm -f conftest.s*
  
