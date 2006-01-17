################################################################################
#
# Makefile  : Bzip2
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <04/03/14 19:28:04 popineau>
#
################################################################################
root_srcdir = ..\..
!ifdef DEVELOPMENT
INCLUDE=$(INCLUDE);$(root_srcdir)\texk.development
!else
INCLUDE=$(INCLUDE);$(root_srcdir)\texk
!endif

USE_GNUW32 = 1
USE_BZIP2 = 1
MAKE_BZIP2 = 1

!include <msvc/common.mak>

programs = $(bzip2dll) $(objdir)\bzip2.exe $(objdir)\bzip2recover.exe
libfiles = $(bzip2lib)
includefiles = bzlib.h
manfiles = bzip2.1
infofiles = manual.info
pdfdocfiles = manual.pdf
docsubdir = bzip2
installinfoflags = --entry="* Bzip2 (bzip2):					An enhance compressor"\
	  --section="Miscellaneous"

objects= $(objdir)\blocksort.obj  \
      $(objdir)\huffman.obj    \
      $(objdir)\crctable.obj   \
      $(objdir)\randtable.obj  \
      $(objdir)\compress.obj   \
      $(objdir)\decompress.obj \
      $(objdir)\bzlib.obj

default: all

all: $(objdir) $(libfiles) $(programs)

lib: $(objdir) $(libfiles)

!ifdef BZIP2_DLL

$(bzip2lib): $(objdir) $(objects)
	$(archive) /DEF $(objects)

$(bzip2dll): $(bzip2lib) $(objects) $(objdir)\bz2lib.res $(gnuw32lib)
	$(link_dll) $(**) $(bzip2lib:.lib=.exp) $(conlibs)
!else

$(bzip2lib): $(objdir) $(objects)
	$(archive) $(objects)
!endif

$(objdir)\bzip2.exe: $(objdir)\bzip2.obj $(bzip2lib)
	$(link) $(**) $(setargvobj) $(conlibs)

$(objdir)\bzip2recover.exe: $(objdir)\bzip2recover.obj $(bzip2lib)
	$(link) $(**) $(setargvobj) $(conlibs)

!include <msvc/config.mak>
!include <msvc/install.mak>

makeinfo_flags = $(makeinfo_flags) --force

install:: install-exec install-lib install-include

test: $(objdir)\bzip2.exe
	@type words1
	.\$(objdir)\bzip2 -1 < sample1.ref > sample1.rb2
	.\$(objdir)\bzip2 -2 < sample2.ref > sample2.rb2
	.\$(objdir)\bzip2 -d < sample1.bz2 > sample1.tst
	.\$(objdir)\bzip2 -d < sample2.bz2 > sample2.tst
	@type words2
	fc sample1.bz2 sample1.rb2 
	fc sample2.bz2 sample2.rb2
	fc sample1.tst sample1.ref
	fc sample2.tst sample2.ref
	@type words3

!include <msvc/clean.mak>

clean::
	-@$(del) sample1.rb2 sample2.rb2 sample1.tst sample2.tst

!include <msvc/rdepend.mak>
!include "./depend.mak"

#
# Local Variables:
# mode: makefile
# End: