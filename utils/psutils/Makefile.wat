# Makefile for PS utilities release 1 patchlevel 17
#
# Copyright (C) Angus J. C. Duggan 1991-1996
# See file LICENSE for details.
#
# updated AJCD 3/1/96
#
# Multiple makefiles for different OSes are generated from a single master
# now.
#
# There are so many incompatible makes around that putting all of the
# non-standard targets explicitly seems to be the only way of ensuring
# portability.

# epsffit fits an epsf file to a given bounding box
# psbook rearranges pages into signatures
# psselect selects page ranges
# pstops performs general page rearrangement and merging
# psnup puts multiple logical pages on one physical page
# psresize scales and moves pages to fit on different paper sizes

PAPER=a4

# Makefile for PSUtils under MSDOS

OS = DOS

BINDIR = \psutils
SCRIPTDIR = $(BINDIR)
INCLUDEDIR = \psutils\include

# Flags for Watcom C Compiler
CC = wcc386
CFLAGS = -dPAPER=\"$(PAPER)\" -dMSDOS -zq -oxt

BIN = psbook.exe psselect.exe pstops.exe epsffit.exe psnup.exe \
	psresize.exe
SHELLSCRIPTS =  
PERLSCRIPTS =     \
	     \
	   
MANPAGES =      \
	    \
	    \
	    \
	  
INCLUDES = md68_0.ps md71_0.ps

all: $(BIN) $(PERLSCRIPTS) $(MANPAGES) $(SHELLSCRIPTS)

psutil.obj: psutil.h patchlev.h pserror.h psutil.c

psspec.obj: psutil.h patchlev.h psspec.h pserror.h psspec.c

pserror.obj: psutil.h patchlev.h pserror.h pserror.c

epsffit.obj: epsffit.c pserror.h patchlev.h

epsffit.exe: epsffit.obj pserror.obj
	wcc386 -epsffite=epsffit.exe pserror.obj epsffit.obj

psnup.exe: psnup.obj psutil.obj psspec.obj pserror.obj
	wcc386 -psnupe=psnup.exe psutil.obj psspec.obj pserror.obj psnup.obj

psnup.obj: psutil.h patchlev.h psspec.h pserror.h psnup.c

psresize.exe: psresize.obj psutil.obj pserror.obj psspec.obj
	wcc386 -psresizee=psresize.exe psutil.obj psspec.obj pserror.obj psresize.obj

psresize.obj: psutil.h patchlev.h psspec.h pserror.h psresize.c

psbook.exe: psbook.obj psutil.obj pserror.obj
	wcc386 -psbooke=psbook.exe psutil.obj pserror.obj psbook.obj

psbook.obj: psutil.h patchlev.h pserror.h psbook.c

psselect.exe: psselect.obj psutil.obj pserror.obj
	wcc386 -psselecte=psselect.exe psutil.obj pserror.obj psselect.obj

psselect.obj: psutil.h patchlev.h pserror.h psselect.c

pstops.exe: pstops.obj psutil.obj psspec.obj pserror.obj
	wcc386 -pstopse=pstops.exe psutil.obj psspec.obj pserror.obj pstops.obj

pstops.obj: psutil.h patchlev.h psspec.h pserror.h pstops.c



clean:
	del *.obj

veryclean realclean: clean
	del $(BIN) $(PERLSCRIPTS) $(MANPAGES)

install: install.bin install.script install.include

install.bin: $(BIN)
	-mkdir $(BINDIR)
	@for %1 in ( $(BIN) ) do copy %1 $(BINDIR)

install.script: $(PERLSCRIPTS)
	-mkdir $(SCRIPTDIR)
	@for %1 in ( $(PERLSCRIPTS) ) do copy %1 $(SCRIPTDIR)

install.include: $(INCLUDES)
	-mkdir $(INCLUDEDIR)
	@for %1 in ( $(INCLUDES) ) do copy %1 $(INCLUDEDIR)

