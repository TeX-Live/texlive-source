*** dviljk/configure.orig	Thu Mar 12 13:20:20 1998
--- dviljk/configure	Tue Mar 17 11:38:20 1998
***************
*** 2140,2145 ****
--- 2140,2146 ----
  s%@LTLIBOBJS@%$LTLIBOBJS%g
  s%@MAINT@%$MAINT%g
  s%@texmfmain@%$texmfmain%g
+ s,	ln -s ,	cp -pr ,
  
  CEOF
  EOF
***************
*** 2273,2279 ****
  s%@srcdir@%$srcdir%g
  s%@top_srcdir@%$top_srcdir%g
  s%@INSTALL@%$INSTALL%g
! " | (eval "$ac_sed_cmds") > $ac_file
  fi; done
  rm -f conftest.s*
  
--- 2274,2280 ----
  s%@srcdir@%$srcdir%g
  s%@top_srcdir@%$top_srcdir%g
  s%@INSTALL@%$INSTALL%g
! " | (eval "$ac_sed_cmds") | sed -e '/^default_texsizes/s,:,;,' > $ac_file
  fi; done
  rm -f conftest.s*
  
