# Makefile for jpeg2ps
# (C) Thomas Merz 1994-1999

VERSION=1.8
DIRNAME = jpeg2ps-$(VERSION)
TARFILE = jpeg2ps-$(VERSION).tar
ZIPFILE = jpeg2ps-$(VERSION).zip

# ----------------------------------------------------------------------------
# Available compile time options:
# -DKNR for Kernighan/Ritchie compilers
# -DA4 gives A4 as default page size, omitting -DA4 gives U.S. letter format
# -DDOS adds DOS and OS/2 support

# The following was reported to work for emx/gcc 0.9c fix04 under OS/2:
# make -f Makefile "CCFLAGS=-c -DA4 -DDOS -O2" "LDFLAGS=-Zexe -s"

CC=gcc
CCFLAGS= -c -DA4
LD=gcc
LDFLAGS=
OBJ=o
EXE=
RM=rm -f

.c.$(OBJ) :
	$(CC) $(CCFLAGS) $*.c

all:	jpeg2ps$(EXE)

# If your system doesn't have getopt(), add the supplied getopt.c
# module below in the jpeg2ps line.

jpeg2ps$(EXE):	jpeg2ps.$(OBJ) readjpeg.$(OBJ) asc85ec.$(OBJ)
	$(LD) $(LDFLAGS) -o jpeg2ps$(EXE) jpeg2ps.$(OBJ) readjpeg.$(OBJ) asc85ec.$(OBJ)

DISTFILES = jpeg2ps.c psimage.h readjpeg.c asc85ec.c getopt.c Makefile jpeg2ps.dsp jpeg2ps.txt descrip.mms

DOSDISTFILES = $(DISTFILES) jpeg2ps.exe

CONVFILES = jpeg2ps.c psimage.h readjpeg.c asc85ec.c getopt.c Makefile jpeg2ps.txt

dist:	$(DOSDISTFILES)
	$(RM) $(ZIPFILE) $(TARFILE).gz;			\
	ln -s . $(DIRNAME);					\
	lineend -d $(CONVFILES);		 	\
	(for i in $(DOSDISTFILES); do				\
		echo $$i;					\
	done) | sed "s;.*;$(DIRNAME)/&;" >distfiles;		\
	zip -9 $(ZIPFILE) `cat distfiles`;			\
	(for i in $(DISTFILES); do				\
		echo $$i;					\
	done) | sed "s;.*;$(DIRNAME)/&;" >distfiles;		\
	lineend -u $(CONVFILES); 			\
	tar cvf  $(TARFILE) `cat distfiles`;			\
	gzip -9 $(TARFILE);					\
	$(RM) $(DIRNAME) distfiles;

clean:
	$(RM) *.$(OBJ) jpeg2ps$(EXE) $(TARFILE) $(ZIPFILE)

jpeg2ps.$(OBJ):		jpeg2ps.c psimage.h

readjpeg.$(OBJ):	readjpeg.c psimage.h

asc85ec.$(OBJ):		asc85ec.c psimage.h

getopt.$(OBJ):		getopt.c
