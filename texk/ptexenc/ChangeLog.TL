ChangeLog.TL: TeX Live (TL) changes for ptexenc
===============================================

2010-04-14  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* Makefile.am (rebuild): New target for forced (re-)build.
	(INCLUDES): Add $(KPATHSEA_INCLUDES) in case we use an installed
	(system) version.
	Add rule to rebuild libkpathsea, if necessary.

2010-04-12  Peter Breitenlohner  <peb@mppmu.mpg.de>

	* ptexenc.c (NOFILE) [MinGW32]: Always define it.
