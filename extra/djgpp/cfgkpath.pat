*** kpathsea/configure.orig	Mon Mar  9 08:23:38 1998
--- kpathsea/configure	Tue Mar 17 11:38:20 1998
***************
*** 2045,2050 ****
--- 2045,2052 ----
  s%@LTLIBOBJS@%$LTLIBOBJS%g
  s%@MAINT@%$MAINT%g
  s%@texmfmain@%$texmfmain%g
+ /^	sed -f texmf.sed/c\\
+ 	sed -e '/^[^%]/s/:/;/g' \\\$(srcdir)/texmf.in | sed -f texmf.sed >\\\$@
  
  CEOF
  EOF
***************
*** 2178,2184 ****
  s%@srcdir@%$srcdir%g
  s%@top_srcdir@%$top_srcdir%g
  s%@INSTALL@%$INSTALL%g
! " | (eval "$ac_sed_cmds") > $ac_file
  fi; done
  rm -f conftest.s*
  
--- 2180,2186 ----
  s%@srcdir@%$srcdir%g
  s%@top_srcdir@%$top_srcdir%g
  s%@INSTALL@%$INSTALL%g
! " | (eval "$ac_sed_cmds") | sed -e '/^default_texsizes/s,:,;,' > $ac_file
  fi; done
  rm -f conftest.s*
  
