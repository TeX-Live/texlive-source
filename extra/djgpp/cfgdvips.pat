*** dvipsk/configure.orig	Thu Mar 12 13:20:44 1998
--- dvipsk/configure	Tue Mar 17 11:38:22 1998
***************
*** 2077,2082 ****
--- 2077,2085 ----
  s%@LTLIBOBJS@%$LTLIBOBJS%g
  s%@MAINT@%$MAINT%g
  s%@texmfmain@%$texmfmain%g
+ s,	ln -s ,	cp -rp ,
+ /^	-PATH=/s,^	-,&PATH_SEPARATOR=: PATH_EXPAND=y ,
+ /^	.*testdata:/s,testdata:,testdata\\\;,g
  
  CEOF
  EOF
***************
*** 2210,2216 ****
  s%@srcdir@%$srcdir%g
  s%@top_srcdir@%$top_srcdir%g
  s%@INSTALL@%$INSTALL%g
! " | (eval "$ac_sed_cmds") > $ac_file
  fi; done
  rm -f conftest.s*
  
--- 2213,2219 ----
  s%@srcdir@%$srcdir%g
  s%@top_srcdir@%$top_srcdir%g
  s%@INSTALL@%$INSTALL%g
! " | (eval "$ac_sed_cmds") | sed -e '/^default_texsizes/s,:,;,' > $ac_file
  fi; done
  rm -f conftest.s*
  
