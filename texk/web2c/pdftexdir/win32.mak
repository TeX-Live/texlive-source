################################################################################
#
# Makefile  : Web2C / pdftexdir
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <04/03/13 13:12:08 popineau>
#
################################################################################
root_srcdir = ..\..\..
!ifdef DEVELOPMENT
INCLUDE=$(INCLUDE);$(root_srcdir)\texk.development
!else
INCLUDE=$(INCLUDE);$(root_srcdir)\texk
!endif


USE_XPDF = 1
USE_ZLIB = 1
USE_PNG = 1
USE_JPEG = 1
USE_TEX = 1
USE_MD5 = 1
USE_GNUW32 = 1
USE_KPATHSEA = 1

!include <msvc/common.mak>

DEFS = -I.. -I$(top_srcdir) $(DEFS) -DMAKE_TEX_DLL -DHAVE_CONFIG_H -DpdfeTeX

objects = $(objdir)\avl.obj $(objdir)\avlstuff.obj $(objdir)\epdf.obj 	     \
	$(objdir)\mapfile.obj                              		     \
	$(objdir)\utils.obj $(objdir)\vfpacket.obj			     \
	$(objdir)\pkin.obj $(objdir)\writefont.obj $(objdir)\writet1.obj     \
	$(objdir)\writet3.obj $(objdir)\writezip.obj $(objdir)\writeenc.obj  \
	$(objdir)\writettf.obj $(objdir)\writejpg.obj $(objdir)\writepng.obj \
	$(objdir)\writeimg.obj \
	$(objdir)\pdftoepdf.obj

default: all

all: $(objdir) libpdftoepdf $(objdir)\libpdf.lib

$(objdir)\libpdf.lib: $(objdir) $(objects) $(md5lib)
	$(archive) $(objects) $(md5lib)

libpdftoepdf: $(objdir) $(objdir)\libpdftoepdf.lib

$(objdir)\libpdftoepdf.lib: $(objdir)\pdftoepdf.obj $(xpdflib)
	$(archive) $(objdir)\pdftoepdf.obj $(xpdflib)

$(objdir)\pdftoepdf.obj: pdftoepdf.cc # $(xpdf)

ptexlib.h: ..\pdftexd.h

..\pdftexd.h:
	-@echo $(verbose) & ( \
		pushd .. & $(make) pdftexd.h & popd \
	)

!include <msvc/config.mak>
!include <msvc/install.mak>

!include <msvc/clean.mak>
!include <msvc/rdepend.mak>

!include "./depend.mak"

#
# Local Variables:
# mode: makefile
# End:
