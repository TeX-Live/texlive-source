################################################################################
#
# Makefile  : 
# Author    : Fabrice Popineau <Fabrice.Popineau@supelec.fr>
# Platform  : Win32, Microsoft VC++ 6.0, depends upon fpTeX 0.5 sources
# Time-stamp: <02/07/18 10:10:10 popineau>
#
################################################################################
root_srcdir = ..\..
INCLUDE=$(INCLUDE);$(root_srcdir)\win32

# This is free software; you can redistribute it and/or modify it under the
# terms of the GNU General Public License, see the file COPYING.

# USE_GNUW32 = 1

!include <msvc/common.mak>

programs =  # $(objdir)\foo.exe 
scripts =
libfiles =
includefiles =
manfiles = # foo.1
infofiles =
#
# Object files
#
objects = # $(objdir)\foo.obj

DEFS = $(DEFS) # -DFOO -I../foo

#
# Main target
#
all: $(programs)

#
# Link target. setargv.obj is provided in the compiler library directory.
#
$(objdir)\foo.exe: $(objdir) $(objects)
	$(link) $(objects) $(LIBS)

!include <msvc/config.mak>
!include <msvc/install.mak>

install:: install-exec # install-lib install-include

!include <msvc/clean.mak>
!include <msvc/rdepend.mak>
!include "./depend.mak"

# End of .mak
#
# Local Variables:
# mode: makefile
# End:
