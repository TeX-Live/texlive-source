################################################################################
#
# Makefile  : libsdef, definitions for using and building libraries
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <03/09/15 12:23:28 popineau>
#
################################################################################

#
# FIXME: rename all the dll/lib into tlxxxx$(maj)$(min)
#

#gifver =		-0.9
#jpgver =		-6b
#pngver =		-1.0.5
#regexver =		-0.12
tiffver =		-v3.5.7
#ttfver =		-1.3
#wwwver =		-5.2.1
#xpdfver =		-0.90
#zlibver =		-1.1.3
#bzip2ver =		-0.9.0c
#t1ver =		-0.9
#gnuw32ver =		-0.1
xpmver =		-3.4k
openldapver =		-2.0.7
netpbmver =		-10.8
rxpver =		-1.3.0

#
# No dlls if compiling statically
#
!if defined(STATIC) || defined(PROFILE)
!undef BMEPS_DLL
!undef BZIP2_DLL
!undef COMPFACE_DLL
!undef CURL_DLL
!undef EXPAT_DLL
!undef FREETYPE2_DLL
!undef FREETYPE_DLL
!undef GETURL_DLL
!undef GIF_DLL
!undef GNUW32_DLL
!undef GSW32_DLL
!undef JPEG_DLL
!undef KPSE_DLL
!undef MD5_DLL
!undef MKTEX_DLL
!undef NETPBM_DLL
!undef OPENLDAP_DLL
!undef PNG_DLL
!undef REGEX_DLL
!undef RXP_DLL
!undef T1_DLL
!undef TEX_DLL
!undef TIFF_DLL
!undef TTF_DLL
!undef UNZIP_DLL
!undef WMF_DLL
!undef WWW_DLL
!undef XPDF_DLL
!undef XPM_DLL
!undef ZLIB_DLL
!else
BMEPS_DLL =		1
BZIP2_DLL =		1
CURL_DLL =		1
COMPFACE_DLL =		1
# EXPAT_DLL =		1
FREETYPE_DLL =		1
FREETYPE2_DLL =		1
GETURL_DLL =		1
GIF_DLL =		1
# GNUW32_DLL = 1
# GSW32_DLL = 1
JPEG_DLL =		1
KPSE_DLL =		1
MKTEX_DLL =		1
NETPBM_DLL =		1
OPENLDAP_DLL =		1
PNG_DLL =		1
REGEX_DLL =		1
T1_DLL =		1
TEX_DLL =		1
TIFF_DLL =		1
TTF_DLL =		1
UNZIP_DLL =		1
WMF_DLL =		1
WWW_DLL =		1
# XPDF_DLL = 1
# MD5_DLL = 1
XPM_DLL =		1
ZLIB_DLL =		1
# RXP_DLL =		1
!endif

library_prefix =	tl$(major_version_number)$(minor_version_number)

#
# The kpathsea library
#
# We need to locate it for several reasons in man.mak
kpathseadir =		$(texk_srcdir)\kpathsea
!ifdef USE_KPATHSEA
!ifdef DEBUG
kpathseaname =		$(library_prefix)kpsed
!else
kpathseaname =		$(library_prefix)kpse
!endif
kpathsealib =		$(kpathseadir)\$(objdir)\$(kpathseaname).lib
!ifdef KPSE_DLL
DEFS =			$(DEFS) -DKPSE_DLL -DKPATHSEA=1
PATH =			$(kpathsea_dir)\$(objdir);$(PATH)
kpathseadll =		$(kpathsealib:.lib=.dll)
!else
DEFS =			$(DEFS) -DKPATHSEA=1
!endif
DEFS =			$(DEFS) -I$(kpathseadir) -I$(kpathseadir)\..
kpathsea =		$(kpathsealib) $(kpathseadll)
!endif

#
# The mktex library
#
!ifdef USE_MKTEX
!ifdef DEBUG
mktexname =		$(library_prefix)mktexd
!else
mktexname =		$(library_prefix)mktex
!endif
mktexdir =		$(texk_srcdir)\contrib
mktexlib =		$(mktexdir)\$(objdir)\$(mktexname).lib
!ifdef MKTEX_DLL
DEFS =			$(DEFS) -DMKTEX_DLL -DMKTEX=1
PATH =			$(mktex_dir)\$(objdir);$(PATH)
mktexdll =		$(mktexlib:.lib=.dll)
!else
DEFS =			$(DEFS) -DMKTEX=1
!endif
DEFS =			$(DEFS) -I$(mktexdir) -I$(mktexdir)\..
mktex =			$(mktexlib) $(mktexdll)
!endif

#
# The bzip2 library
#
!ifdef USE_BZIP2
!ifdef DEBUG
bzip2name =		$(library_prefix)bzip2d
!else
bzip2name =		$(library_prefix)bzip2
!endif
bzip2dir =		$(utils_srcdir)\bzip2$(bzip2ver)
bzip2lib =		$(bzip2dir)\$(objdir)\$(bzip2name).lib
!ifdef BZIP2_DLL
bzip2dll =		$(bzip2lib:.lib=.dll)
DEFS =			$(DEFS) -DBZIP2_DLL
PATH =			$(bzip2dir)\$(objdir);$(PATH)
!endif
DEFS =			$(DEFS) -I$(bzip2dir)
bzip2 =			$(bzip2lib) $(bzip2dll)
!endif

#
# The md5 library
#
!ifdef USE_MD5
!ifdef DEBUG
md5name =		$(library_prefix)md5d
!else
md5name =		$(library_prefix)md5
!endif
md5dir =		$(libs_srcdir)\md5$(md5ver)
md5lib =		$(md5dir)\$(objdir)\$(md5name).lib
!ifdef MD5_DLL
md5dll =		$(md5lib:.lib=.dll)
DEFS =			$(DEFS) -DMD5_DLL
PATH =			$(md5dir)\$(objdir);$(PATH)
!endif
DEFS =			$(DEFS) -I$(md5dir)
md5 =			$(md5lib) $(md5dll)
!endif

#
# The gifreader library
#
!ifdef USE_GIF
!ifdef DEBUG
gifname =		$(library_prefix)gifrd
!else
gifname =		$(library_prefix)gifr
!endif
gifdir =		$(libs_srcdir)\gifreader$(gifver)
giflib =		$(gifdir)\$(objdir)\$(gifname).lib
!ifdef GIF_DLL
gifdll =		$(giflib:.lib=.dll)
DEFS =			$(DEFS) -DGIF_DLL
PATH =			$(gifdir)\$(objdir);$(PATH)
!endif
DEFS =			$(DEFS) -I$(gifdir)
gif =			$(giflib) $(gifdll)
!endif

#
# The Gnu W32 unix compatibility library
#
!ifdef USE_GNUW32
!ifdef DEBUG
gnuw32name =		$(library_prefix)gnu
!else
gnuw32name =		$(library_prefix)gnu
!endif
gnuw32dir =		$(libs_srcdir)\libgnuw32
gnuw32lib =		$(gnuw32dir)\$(objdir)\$(gnuw32name).lib
!ifdef GNUW32_DLL
gnuw32dll =		$(gnuw32lib:.lib=.dll)
DEFS =			$(DEFS) -DGNUW32_DLL
PATH =			$(gnuw32dir)\$(objdir);$(PATH)
!endif
DEFS =			$(DEFS) -I$(gnuw32dir)
!ifndef USE_KPATHSEA
DEFS =			$(DEFS) -I$(kpathseadir) -I$(kpathseadir)\..
!endif
gnuw32 =		$(gnuw32lib) $(gnuw32dll)
!endif

#
# The GS W32 unix compatibility library
#
!ifdef USE_GSW32
!ifdef DEBUG
gsw32name =		$(library_prefix)gsd
!else
gsw32name =		$(library_prefix)gs
!endif
gsw32dir =		$(libs_srcdir)\libgsw32
gsw32lib =		$(gsw32dir)\$(objdir)\$(gsw32name).lib
!ifdef GSW32_DLL
gsw32dll =		$(gsw32lib:.lib=.dll)
DEFS =			$(DEFS) -DGSW32_DLL
PATH =			$(gsw32dir)\$(objdir);$(PATH)
!endif
DEFS =			$(DEFS) -I$(gsw32dir) -I$(root_srcdir)\gstools\ghostscript\src
!ifndef USE_KPATHSEA
DEFS =			$(DEFS) -I$(kpathseadir)
!endif
gsw32 =			$(gsw32lib) $(gsw32dll)
!endif

#
# The jpeg library
#
!ifdef USE_JPEG
!ifdef DEBUG
jpegname =		$(library_prefix)jpegd
!else
jpegname =		$(library_prefix)jpeg
!endif
jpegdir =		$(libs_srcdir)\jpeg$(jpegver)
jpeglib =		$(jpegdir)\$(objdir)\$(jpegname).lib
!ifdef JPEG_DLL
jpegdll =		$(jpeglib:.lib=.dll)
DEFS =			$(DEFS) -DJPEG_DLL
PATH =			$(jpegdir)\$(objdir);$(PATH)
!endif
DEFS =			$(DEFS) -I$(jpegdir)
jpeg =			$(jpeglib) $(jpegdll)
!endif

#
# The png library
#
!ifdef USE_PNG
!ifdef DEBUG
pngname =		$(library_prefix)pngd
!else
pngname =		$(library_prefix)png
!endif
pngdir =		$(libs_srcdir)\libpng$(pngver)
pnglib =		$(pngdir)\$(objdir)\$(pngname).lib
!ifdef PNG_DLL
pngdll =		$(pnglib:.lib=.dll)
DEFS =			$(DEFS) -DPNG_DLL
PATH =			$(pngdir)\$(objdir);$(PATH)
!endif
DEFS =			$(DEFS) -DPNG_NO_GLOBAL_ARRAYS -DPNG_USE_PNGVCRD -D__FLAT__ -I$(pngdir)
png =			$(pnglib) $(pngdll)
!endif

#
# The regex library
#
!ifdef USE_REGEX
!ifdef DEBUG
regexname =		$(library_prefix)regexd
!else
regexname =		$(library_prefix)regex
!endif
regexdir =		$(libs_srcdir)\regex$(regexver)
regexlib =		$(regexdir)\$(objdir)\$(regexname).lib
!ifdef REGEX_DLL
regexdll =		$(regexlib:.lib=.dll)
DEFS =			$(DEFS) -DREGEX_DLL
PATH =			$(regexdir)\$(objdir);$(PATH)
!endif
DEFS =			$(DEFS) -I$(regexdir)
regex =			$(regexlib) $(regexdll)
!endif

#
# The Type1 library
#
!ifdef USE_T1
!ifdef DEBUG
t1name =		$(library_prefix)t1d
!else
t1name =		$(library_prefix)t1
!endif
t1dir =			$(libs_srcdir)\T1$(t1ver)\lib\t1lib
t1lib =			$(t1dir)\$(objdir)\$(t1name).lib
!ifdef T1_DLL
t1dll =			$(t1lib:.lib=.dll)
DEFS =			$(DEFS) -DT1_DLL
PATH =			$(t1dir)\$(objdir);$(PATH)
!endif
DEFS =			$(DEFS) -I$(t1dir)
t1 =			$(t1lib) $(t1dll)
!endif

#
# The TIFF library
#
!ifdef USE_TIFF
!ifdef DEBUG
tiffname =		$(library_prefix)tiffd
!else
tiffname =		$(library_prefix)tiff
!endif
tiffdir =		$(graphics_srcdir)\tiff$(tiffver)\libtiff
tifflib =		$(tiffdir)\$(objdir)\$(tiffname).lib
!ifdef TIFF_DLL
tiffdll =		$(tifflib:.lib=.dll)
DEFS =			$(DEFS) -DTIFF_DLL
PATH =			$(tiffdir)\$(objdir);$(PATH)
!endif
DEFS =			$(DEFS) -I$(tiffdir)
tiff =			$(tifflib) $(tiffdll)
!endif

#
# The TrueType library
#
!ifdef USE_TTF
!ifdef DEBUG
ttfname =		$(library_prefix)ttfd
!else
ttfname =		$(library_prefix)ttf
!endif
ttfdir =		$(libs_srcdir)\libttf$(ttfver)
ttflib =		$(ttfdir)\$(objdir)\$(ttfname).lib
!ifdef TTF_DLL
ttfdll =		$(ttflib:.lib=.dll)
DEFS =			$(DEFS) -DTTF_DLL
PATH =			$(ttfdir)\$(objdir);$(PATH)
!endif
DEFS =			$(DEFS) -I$(ttfdir)
ttf =			$(ttflib) $(ttfdll)
!endif

#
# The Freetype library
#
!ifdef USE_FREETYPE
!ifdef DEBUG
freetypename =		$(library_prefix)ftd
!else
freetypename =		$(library_prefix)ft
!endif
freetypedir =		$(libs_srcdir)\freetype$(freetypever)
freetypelib =		$(freetypedir)\lib\$(objdir)\$(freetypename).lib
!ifdef FREETYPE_DLL
freetypedll =		$(freetypelib:.lib=.dll)
DEFS =			$(DEFS) -DFREETYPE_DLL
PATH =			$(freetypedir)\$(objdir);$(PATH)
!endif
DEFS =			$(DEFS) -I$(freetypedir)\lib
freetype =		$(freetypelib) $(freetypedll)
!endif

#
# The Freetype2 library
#
!ifdef USE_FREETYPE2
!ifdef DEBUG
freetype2name =		$(library_prefix)freetype2d
!else
freetype2name =		$(library_prefix)freetype2
!endif
freetype2dir =		$(libs_srcdir)\freetype2$(freetype2ver)
freetype2lib =		$(freetype2dir)\src\$(objdir)\$(freetype2name).lib
!ifdef FREETYPE2_DLL
freetype2dll =		$(freetype2lib:.lib=.dll)
DEFS =			$(DEFS) -DFREETYPE2_DLL
PATH =			$(freetype2dir)\$(objdir);$(PATH)
!endif
DEFS =			$(DEFS) -I$(freetype2dir)\include
freetype2 =		$(freetype2lib) $(freetype2dll)
!endif

#
# The Curl library
#
!ifdef USE_CURL
!ifdef DEBUG
curlname =		$(library_prefix)curld
!else
curlname =		$(library_prefix)curl
!endif
curldir =		$(libs_srcdir)\curl$(curlver)
curllib =		$(curldir)\lib\$(objdir)\$(curlname).lib
!ifdef CURL_DLL
curldll =		$(curllib:.lib=.dll)
DEFS =			$(DEFS) -DCURL_DLL
PATH =			$(curldir)\$(objdir);$(PATH)
!endif
DEFS =			$(DEFS) -I$(curldir)\include
curl =			$(curllib) $(curldll)
!endif

#
# The www library
#
!ifdef USE_WWW
!ifdef DEBUG
wwwname =		$(library_prefix)wwwd
!else
wwwname =		$(library_prefix)www
!endif
wwwdir =		$(libs_srcdir)\libwww$(wwwver)
wwwlib =		$(wwwdir)\$(objdir)\$(wwwname).lib
!ifdef WWW_DLL
wwwdll =		$(wwwlib:.lib=.dll)
DEFS =			$(DEFS) -DWWW_DLL
# WWWLIB_FLAGS=-DWWW_WIN_DLL -DWWW_WIN_ASYNC
PATH =			$(wwwdir)\$(objdir);$(PATH)
!endif
DEFS =			$(DEFS) -I$(wwwdir)
www =			$(wwwlib) $(wwwdll)
!endif

#
# The xpdf library
#
!ifdef USE_XPDF
!ifdef DEBUG
xpdfname =		$(library_prefix)xpdfd
!else
xpdfname =		$(library_prefix)xpdf
!endif
xpdfdir =		$(libs_srcdir)\xpdf$(xpdfver)
xpdflib =		$(xpdfdir)\xpdf\$(objdir)\$(xpdfname).lib
!ifdef XPDF_DLL
xpdfdll =		$(xpdflib:.lib=.dll)
PATH =			$(xpdfdir)\xpdf\$(objdir);$(PATH)
!endif
DEFS =			$(DEFS) -I$(xpdfdir) -I$(xpdfdir)\xpdf -I$(xpdfdir)\Goo
xpdf =			$(xpdflib) $(xpdfdll)
!endif

#
# The zlib library
#
!ifdef USE_ZLIB
!ifdef DEBUG
zlibname =		$(library_prefix)zlibd
!else
zlibname =		$(library_prefix)zlib
!endif
zlibdir =		$(libs_srcdir)\zlib$(zlibver)
zliblib =		$(zlibdir)\$(objdir)\$(zlibname).lib
!ifdef ZLIB_DLL
zlibdll =		$(zliblib:.lib=.dll)
DEFS =			$(DEFS) -DZLIB_DLL
PATH =			$(zlibdir)\$(objdir);$(PATH)
!endif
DEFS =			$(DEFS) -I$(zlibdir)
zlib =			$(zliblib) $(zlibdll)
!endif

#
# The expat library
#
!ifdef USE_EXPAT
!ifdef DEBUG
expatname =		$(library_prefix)expatd
!else
expatname =		$(library_prefix)expat
!endif
expatdir =		$(libs_srcdir)\expat$(expatver)\lib
expatlib =		$(expatdir)\$(objdir)\$(expatname).lib
!ifdef EXPAT_DLL
expatdll =		$(expatlib:.lib=.dll)
DEFS =			$(DEFS) -DEXPAT_DLL
PATH =			$(expatdir)\$(objdir);$(PATH)
!endif
DEFS =			$(DEFS) -I$(expatdir)
expat =			$(expatlib) $(expatdll)
!endif

#
# The unzip library
#
!ifdef USE_UNZIP
!ifdef DEBUG
unzipname =		$(library_prefix)unzipd
!else
unzipname =		$(library_prefix)unzip
!endif
unzipdir =		$(libs_srcdir)\unzip$(unzipver)
unziplib =		$(unzipdir)\$(objdir)\$(unzipname).lib
!ifdef UNZIP_DLL
unzipdll =		$(unziplib:.lib=.dll)
DEFS =			$(DEFS) -DUNZIP_DLL
PATH =			$(unzipdir)\$(objdir);$(PATH)
!endif
DEFS =			$(DEFS) -I$(unzipdir) -I$(unzipdir)\windll
unzip =			$(unziplib) $(unzipdll)
!endif

#
# The geturl library
#
!ifdef USE_GETURL
!ifdef DEBUG
geturlname =		$(library_prefix)geturld
!else
geturlname =		$(library_prefix)geturl
!endif
geturldir =		$(libs_srcdir)\geturl$(geturlver)
geturllib =		$(geturldir)\$(objdir)\$(geturlname).lib
!ifdef GETURL_DLL
geturldll =		$(geturllib:.lib=.dll)
DEFS =			$(DEFS) -DGETURL_DLL
PATH =			$(geturldir)\$(objdir);$(PATH)
!endif
DEFS =			$(DEFS) -I$(geturldir)
geturl =		$(geturllib) $(geturldll)
!endif

#
# The netpbm library
#
!ifdef USE_NETPBM
!ifdef DEBUG
netpbmname =		$(library_prefix)netpbmd
!else
netpbmname =		$(library_prefix)netpbm
!endif
netpbmdir =		$(graphics_srcdir)\netpbm$(netpbmver)
netpbmlib =		$(netpbmdir)\lib\$(objdir)\$(netpbmname).lib
!ifdef NETPBM_DLL
netpbmdll =		$(netpbmlib:.lib=.dll)
DEFS =			$(DEFS) -DNETPBM_DLL # -DPNM_DLL -DPGM_DLL -DPPM_DLL
PATH =			$(netpbmdir)\lib\$(objdir);$(PATH)
!endif
DEFS =			$(DEFS) -I$(netpbmdir)\lib -I$(netpbmdir)
netpbm =		$(netpbmlib) $(netpbmdll)
!endif

#
# The wmf libraries
#
!ifdef USE_WMF
!ifdef DEBUG
wmfaname =		$(library_prefix)wmfad
wmfiname =		$(library_prefix)wmfid
!else
wmfaname =		$(library_prefix)wmfa
wmfiname =		$(library_prefix)wmfi
!endif
wmfdir =		$(utils_srcdir)\libwmf
wmfapidir =		$(wmfdir)\src\api
wmfapilib =		$(wmfdir)\$(objdir)\$(wmfaname).lib
wmfapidll =		$(wmfapilib:.lib=.dll)
wmfipadir =		$(wmfdir)\src\ipa
wmfipalib =		$(wmfdir)\$(objdir)\$(wmfiname).lib
wmfipadll =		$(wmfipalib:.lib=.dll)
wmflib =		$(wmfapilib) $(wmfipalib)
!ifdef WMF_DLL
wmfdll =		$(wmfapidll) $(wmfipadll)
DEFS =			$(DEFS) -DWMF_DLL
PATH =			$(wmfapidir)\$(objdir);$(wmfipadir)\$(objdir);$(PATH)
!endif
DEFS =			$(DEFS) -I$(wmfapidir) -I$(wmfipadir) -I$(wmfdir)\src -I$(wmfdir)\include
wmfapi =		$(wmfapilib) $(wmfapidll)
wmfipa =		$(wmfipalib) $(wmfipadll)
wmf =			$(wmflib) $(wmfdll)
!endif

#
# The XPM library
#
!ifdef USE_XPM
!ifdef DEBUG
xpmname =		$(library_prefix)xpmd
!else
xpmname =		$(library_prefix)xpm
!endif
xpmdir =		$(graphics_srcdir)\xpm$(xpmver)
xpmlib =		$(xpmdir)\lib\$(objdir)\$(xpmname).lib
!ifdef XPM_DLL
xpmdll =		$(xpmlib:.lib=.dll)
DEFS =			$(DEFS) -DXPM_DLL
PATH =			$(xpmdir)\lib\$(objdir);$(PATH)
!endif
DEFS =			$(DEFS) -DFOR_MSW -I$(xpmdir)
xpm =			$(xpmlib) $(xpmdll)
!endif

#
# The openldap library
#
!ifdef USE_OPENLDAP
openldapdir =		$(gnuroot_srcdir)\openldap$(openldapver)
openlavllib =		$(openldapdir)\libraries\libavl\$(objdir)\openlavl.lib
openlutillib =		$(openldapdir)\libraries\liblutil\$(objdir)\openlutil.lib
openlberlib =		$(openldapdir)\libraries\liblber\$(objdir)\openlber.lib
openldaplib =		$(openldapdir)\libraries\libldap\$(objdir)\openldap.lib
openldap_rlib =		$(openldapdir)\libraries\libldap_r\$(objdir)\openldap_r.lib
openldbmlib =		$(openldapdir)\libraries\libldbm\$(objdir)\openldbm.lib
openldiflib =		$(openldapdir)\libraries\libldif\$(objdir)\openldif.lib
openlunicodelib =		$(openldapdir)\libraries\liblunicode\$(objdir)\openlunicode.lib
!ifdef OPENLDAP_DLL
openlavldll =		$(openldapdir)\libraries\libavl\$(objdir)\openlavl.dll
openlutildll =		$(openldapdir)\libraries\liblutil\$(objdir)\openlutil.dll
openlberdll =		$(openldapdir)\libraries\liblber\$(objdir)\openlber.dll
openldapdll =		$(openldapdir)\libraries\libldap\$(objdir)\openldap.dll
openldap_rdll =		$(openldapdir)\libraries\libldap_r\$(objdir)\openldap_r.dll
openldbmdll =		$(openldapdir)\libraries\libldbm\$(objdir)\openldbm.dll
openldifdll =		$(openldapdir)\libraries\libldif\$(objdir)\openldif.dll
openlunicodedll =		$(openldapdir)\libraries\liblunicode\$(objdir)\openlunicode.dll
DEFS =			$(DEFS) -DOPENLDAP_DLL
PATH =			$(openldapdir)\$(objdir);$(PATH)
!endif
DEFS =			$(DEFS) -I$(openldapdir)\include
openlavl =		$(openlavllib) $(openlavldll)
openlutil =		$(openlutillib) $(openlutildll)
openlber =		$(openlberlib) $(openlberdll)
openldap =		$(openldaplib) $(openldapdll)
openldap_r =		$(openldap_rlib) $(openldap_rdll)
openldbm =		$(openldbmlib) $(openldbmdll)
openldif =		$(openldiflib) $(openldifdll)
openlunicode =		$(openlunicodelib) $(openlunicodedll)
!endif

#
# The compface library
#
!ifdef USE_COMPFACE
!ifdef DEBUG
compfacename =		$(library_prefix)cfaced
!else
compfacename =		$(library_prefix)cface
!endif
compfacedir =		$(gnuroot_srcdir)\compface$(compfacever)
compfacelib =		$(compfacedir)\$(objdir)\$(compfacename).lib
!ifdef COMPFACE_DLL
compfacedll =		$(compfacelib:.lib=.dll)
DEFS =			$(DEFS) -DCOMPFACE_DLL
PATH =			$(compfacedir)\$(objdir);$(PATH)
!endif
DEFS =			$(DEFS) -I$(compfacedir)
compface =		$(compfacelib) $(compfacedll)
!endif

#
# The RXP library
#
!ifdef USE_RXP
!ifdef DEBUG
rxpname =		$(library_prefix)rxpd
!else
rxpname =		$(library_prefix)rxp
!endif
rxpdir =		$(gnuroot_srcdir)\rxp$(rxpver)
rxplib =		$(rxpdir)\$(objdir)\$(rxpname).lib
!ifdef RXP_DLL
rxpdll =		$(rxplib:.lib=.dll)
DEFS =			$(DEFS) -DRXP_DLL
PATH =			$(rxpdir)\$(objdir);$(PATH)
!endif
DEFS =			$(DEFS) -I$(rxpdir)
rxp =			$(rxplib) $(rxpdll)
!endif

#
# The RLE/URT library
#
!ifdef USE_RLE
!ifdef DEBUG
rlename =		$(library_prefix)rled
!else
rlename =		$(library_prefix)rle
!endif
rledir =		$(graphics_srcdir)\netpbm$(netpbmver)\urt$(rlever)
rlelib =		$(rledir)\$(objdir)\$(rlename).lib
!ifdef RLE_DLL
rledll =		$(rlelib:.lib=.dll)
DEFS =			$(DEFS) -DRLE_DLL
PATH =			$(rledir)\$(objdir);$(PATH)
!endif
DEFS =			$(DEFS) -I$(rledir)
rle =			$(rlelib) $(rledll)
!endif

#
# The bmeps library
#
!ifdef USE_BMEPS
!ifdef DEBUG
bmepsname =		$(library_prefix)bmepsd
!else
bmepsname =		$(library_prefix)bmeps
!endif
bmepsdir =		$(graphics_srcdir)\bmeps$(bmepsver)
bmepslib =		$(bmepsdir)\$(objdir)\$(bmepsname).lib
!ifdef BMEPS_DLL
bmepsdll =		$(bmepslib:.lib=.dll)
DEFS =			$(DEFS) -DBMEPS_DLL
PATH =			$(bmepsdir)\$(objdir);$(PATH)
!endif
DEFS =			$(DEFS) -I$(bmepsdir)
bmeps =			$(bmepslib) $(bmepsdll)
!endif

#
# The TeX engines
#
!ifdef USE_TEX
!ifdef TEX_DLL
DEFS =			$(DEFS) -DTEX_DLL
!endif
!endif

#
# Setargv expands arguments
# Unfortunately, when linking with msvcrt.dll (as it is recommended under NT)
# it is not possible to provide his own setargv() routine. Globbing is hardcoded
# in the dll and triggered only if you link in the dummy MS setargv.obj
#
setargvobj =		setargv.obj

# End of libsdef.mak
#
# Local Variables:
# mode: Makefile
# End:
