################################################################################
#
# Makefile  : fpTeX libraries
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <03/10/12 18:50:34 popineau>
#
################################################################################
root_srcdir = ..
!ifdef DEVELOPMENT
INCLUDE=$(INCLUDE);$(root_srcdir)\texk.development
!else
INCLUDE=$(INCLUDE);$(root_srcdir)\texk
!endif

!include <msvc/common.mak>

prereqfiles = \
	"$(MSDEV)\Visual Studio .NET Enterprise Architect 2003 - English\MSVCP71.dll" \
	"$(MSDEV)\Visual Studio .NET Enterprise Architect 2003 - English\MSVCR71.dll" \
	"c:\Windows\System32\MFC71.dll" \
	c:\Local\Perl\bin\perl58.dll \
#	$(jpegdll) \
#	$(pngdll) \
#	$(zlibdll) \
#	$(bzip2dll)

# Package subdirectories.
subdirs = \
	libgnuw32	   \
	libgsw32	   \
	regex$(regexver)   \
	zlib$(zlibver)     \
	libpng$(pngver)    \
	jpeg$(jpgver)      \
#	libtiff$(tiffver)  \
	xpdf$(xpdfver)     \
	freetype2	   \
	libttf$(ttfver)    \
#	gifreader$(gifver) \
#	curl		   \
	md5		   \
	expat$(expatver)   \
	geturl$(geturlver) \
#	unzip		   \
#	T1$(t1ver)

!include <msvc/subdirs.mak>

install:: prerequisites

prerequisites: $(prereqfiles)
	for %%f in ($(prereqfiles)) do $(copy) %%f $(bindir)

!include <msvc/clean.mak>

# Local Variables:
# mode: Makefile
# End:
