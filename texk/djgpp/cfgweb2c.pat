*** web2c/configure.orig	Mon Mar  9 08:23:40 1998
--- web2c/configure	Tue Mar 17 11:38:24 1998
***************
*** 4271,4276 ****
--- 4271,4286 ----
  s%@zlib@%$zlib%g
  s%@pdftexlibsdep@%$pdftexlibsdep%g
  s%@zlibinc@%$zlibinc%g
+ s,\.gft,.gf,g
+ /^	MAKEMPX_BINDIR/s,^	,&PATH_SEPARATOR=: PATH_EXPAND=y ,g
+ /^	-PATH=/s,^	-,&PATH_SEPARATOR=: PATH_EXPAND=y ,g
+ /\\\$(LN).*initex/s,\\\$(LN),ln -s,g
+ /\\\$(LN).*iniomega/s,\\\$(LN),ln -s,g
+ /\\\$(LN).*inimf/s,\\\$(LN),ln -s,g
+ /\\\$(LN).*inimpost/s,\\\$(LN),ln -s,g
+ /\\\$\\\$base/s,\\\$(LN),ln -s,g
+ s,pooltype-check,poolt-check,
+ /TFMFONTS=/s,tests:,tests\\\;,
  
  CEOF
  EOF
***************
*** 4406,4412 ****
  s%@srcdir@%$srcdir%g
  s%@top_srcdir@%$top_srcdir%g
  s%@INSTALL@%$INSTALL%g
! " | (eval "$ac_sed_cmds") > $ac_file
  fi; done
  rm -f conftest.s*
  
--- 4416,4422 ----
  s%@srcdir@%$srcdir%g
  s%@top_srcdir@%$top_srcdir%g
  s%@INSTALL@%$INSTALL%g
! " | (eval "$ac_sed_cmds") | sed -e '/^default_texsizes/s,:,;,' > $ac_file
  fi; done
  rm -f conftest.s*
  
