################################################################################
#
# Makefile  : libsrul, dependency rules to build libraries
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <03/07/22 05:32:42 popineau>
#
################################################################################

!if (defined(USE_GNUW32) && !defined(MAKE_GNUW32))
$(gnuw32lib):
	pushd $(gnuw32dir) & $(make) lib & popd
!endif

!if (defined(USE_GSW32) && !defined(MAKE_GSW32))
$(gsw32lib):
	pushd $(gsw32dir) & $(make) lib & popd
!endif

!if (defined(USE_MKTEX) && !defined(MAKE_MKTEX))
$(mktexlib):
	pushd $(mktexdir) & $(make) lib & popd
!endif

!if (defined(USE_KPATHSEA) && !defined(MAKE_KPATHSEA))
$(kpathsealib):
	pushd $(kpathseadir) & $(make) lib & popd
!endif

!if (defined(USE_ZLIB) && !defined(MAKE_ZLIB))
$(zliblib):
	pushd $(zlibdir) & $(make) lib & popd
!endif

!if (defined(USE_BZIP2) && !defined(MAKE_BZIP2))
$(bzip2lib):
	pushd $(bzip2dir) & $(make) lib & popd
!endif

!if (defined(USE_MD5) && !defined(MAKE_MD5))
$(md5lib):
	pushd $(md5dir) & $(make) lib & popd
!endif

!if (defined(USE_PNG) && !defined(MAKE_PNG))
$(pnglib):
	pushd $(pngdir) & $(make) lib & popd
!endif

!if (defined(USE_XPDF) && !defined(MAKE_XPDF))
$(xpdflib):
	pushd $(xpdfdir) & $(make) lib & popd
!endif

!if (defined(USE_TIFF) && !defined(MAKE_TIFF))
$(tifflib):
	pushd $(tiffdir) & $(make) lib & popd
!endif

!if (defined(USE_JPEG) && !defined(MAKE_JPEG))
$(jpeglib):
	pushd $(jpegdir) & $(make) lib & popd
!endif

!if (defined(USE_GIF) && !defined(MAKE_GIF))
$(giflib):
	pushd $(gifdir) & $(make) lib & popd
!endif

!if (defined(USE_REGEX) && !defined(MAKE_REGEX))
$(regexlib):
	pushd $(regexdir) & $(make) lib & popd
!endif

!if (defined(USE_WWW) && !defined(MAKE_WWW))
$(wwwlib):
	pushd $(wwwdir) & $(make) lib & popd
!endif

!if (defined(USE_EXPAT) && !defined(MAKE_EXPAT))
$(expatlib):
	pushd $(expatdir) & $(make) lib & popd
!endif

!if (defined(USE_GETURL) && !defined(MAKE_GETURL))
$(geturllib):
	pushd $(geturldir) & $(make) lib & popd
!endif

!if (defined(USE_UNZIP) && !defined(MAKE_UNZIP))
$(unziplib):
	pushd $(unzipdir) & $(make) lib & popd
!endif

!if (defined(USE_NETPBM) && !defined(MAKE_NETPBM))
$(netpbmlib):
	pushd $(netpbmdir) & $(make) lib & popd
!endif

!if (defined(USE_WMF) && !defined(MAKE_WMF))
$(wmflib):
	pushd $(wmfdir) & $(make) lib & popd
!endif

!if (defined(USE_FREETYPE) && !defined(MAKE_FREETYPE))
$(freetypelib):
	pushd $(freetypedir) & $(make) lib & popd
!endif

!if (defined(USE_FREETYPE2) && !defined(MAKE_FREETYPE2))
$(freetype2lib):
	pushd $(freetype2dir) & $(make) lib & popd
!endif

!if (defined(USE_CURL) && !defined(MAKE_CURL))
$(curllib):
	pushd $(curldir) & $(make) lib & popd
!endif

!if (defined(USE_RLE) && !defined(MAKE_RLE))
$(rlelib):
	pushd $(rledir) & $(make) lib & popd
!endif

!if (defined(USE_COMPFACE) && !defined(MAKE_COMPFACE))
$(compfacelib):
	pushd $(compfacedir) & $(make) lib & popd
!endif

!if (defined(USE_RXP) && !defined(MAKE_RXP))
$(rxplib):
	pushd $(rxpdir) & $(make) lib & popd
!endif

!if (defined(USE_BMEPS) && !defined(MAKE_BMEPS))
$(bmepslib):
	pushd $(bmepsdir) & $(make) lib & popd
!endif

!if (defined(USE_XPM) && !defined(MAKE_XPM))
$(xpmlib):
	pushd $(xpmdir) & $(make) lib & popd
!endif

# End of libsrul.mak
#
# Local Variables:
# mode: Makefile
# End:
