#
# Top Level Makefile for TeXLive for Win32
#
root_srcdir = .
INCLUDE=$(INCLUDE);$(root_srcdir)\texk

USE_JPEG = 1
USE_PNG = 1
USE_ZLIB = 1
USE_BZIP2 = 1

!include <msvc/common.mak>

!if defined(XEMTEX)
subdirs = xemacs gstools graphics perl

!elseif defined(NETPBM)

subdirs = graphics

!else

# Kpathsea needs to be build before 
!ifdef DEVELOPMENT
subdirs = libs    \
	  utils   \
	  graphics \
	  texk.development
!else
subdirs = libs          \
	  utils		\
	  graphics      \
	  texk
!endif

!endif

!ifdef XEMTEX
install:: $(prefix) 

$(prefix):
	-if not exist $(prefix) $(mkdir) $(prefix)
!endif

!include <msvc/subdirs.mak>
!include <msvc/clean.mak>

#
# Local Variables:
# mode: makefile
# End:
