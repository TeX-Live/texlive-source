################################################################################
#
# Makefile  : Gzip
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <04/03/10 23:41:05 popineau>
#
################################################################################
root_srcdir = ..\..
!ifdef DEVELOPMENT
INCLUDE=$(INCLUDE);$(root_srcdir)\texk.development
!else
INCLUDE=$(INCLUDE);$(root_srcdir)\texk
!endif

# This is free software; you can redistribute it and/or modify it under the
# terms of the GNU General Public License, see the file COPYING.

# This makefile is suitable for NTFS only. To build a gzip executable suitable
# for a FAT file system, add -DNTFAT to DEFS.

# To build debug version, define environment variable DEBUG, or include a
# -DDEBUG on the commandline (i.e.: nmake -DDEBUG)

USE_GNUW32 = 1

!include <msvc/common.mak>

DEFS = $(DEFS) -DKPATHSEA=1

programs = $(objdir)\gzip.exe $(objdir)\gunzip.exe
manfiles = gzip.1
infofiles = gzip.info
pdfdocfiles = gzip.pdf
docsubdir = gzip
installinfoflags = --entry="* Gzip (gzip):					The GNU compressor"\
	  --section="Miscellaneous"
#
# Object files
#
objects = $(objdir)\gzip.obj $(objdir)\zip.obj $(objdir)\deflate.obj     \
	$(objdir)\trees.obj $(objdir)\bits.obj $(objdir)\unzip.obj    \
	$(objdir)\inflate.obj $(objdir)\util.obj $(objdir)\crypt.obj  \
	$(objdir)\lzw.obj $(objdir)\unlzw.obj $(objdir)\unpack.obj    \
	$(objdir)\unlzh.obj

#
# Main target
#
default: all

all: $(objdir) $(programs)

#
# Link target. setargv.obj is provided in the compiler library directory.
#
$(objdir)\gzip.exe: $(objects) $(gnuw32lib)
	$(link) $(**) $(conlibs)

$(objdir)\gunzip.exe: $(objdir)\gzip.exe
	$(copy) $(**) $@

!include <msvc/config.mak>
!include <msvc/install.mak>

install:: install-exec

test: check

check:	$(programs)
	.\$(objdir)\gzip -6 < $(srcdir)/texinfo.tex > _gztest.gz
	@echo The next number should be 30890
	wc -c < _gztest.gz
	-@$(del) _gztest
	.\$(objdir)\gzip -d _gztest.gz
	-@fc _gztest $(srcdir)\texinfo.tex
	-@if ERRORLEVEL 1 \
	   echo FAILED gzip test: incorrect size; \
	-@$(del) _gztest

!include <msvc/clean.mak>
!include <msvc/rdepend.mak>
!include "./depend.mak"

#
# Local Variables:
# mode: makefile
# End:
