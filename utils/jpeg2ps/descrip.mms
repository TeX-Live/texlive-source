# Makefile for jpeg2ps
# (C) Thomas Merz 1994-1999
# Unsupported VMS Makefile for mms, initially provided by
# Rolf Niepraschk (niepraschk@ptb.de )

# ----------------------------------------------------------------------------
# VMS version
#
# throw out /DEFINE=A4 if you want letter format as default size
# throw out -DA4 if you want letter format as default size
# use /DEFINE=KNR for Kernighan/Ritchie compilers

CC=cc
CCOPT= /DEFINE=A4/PREFIX=ALL/NOWARN
LDOPT=
OBJ=OBJ
EXE=.EXE
RM=DEL/LOG

.c.$(OBJ) :
	$(CC) $(CCOPT) $*.c

all :	jpeg2ps$(EXE)
	@ !

jpeg2ps$(EXE) :	jpeg2ps.$(OBJ) readjpeg.$(OBJ) asc85ec.$(OBJ) getopt.$(OBJ)
		LINK $(LDOPT) /EXE=$@ $+
		

clean :
        @ $ IF F$SEARCH("*.$(OBJ)",).NES."" THEN  $(RM) *.$(OBJ);*
        @ $ IF F$SEARCH("jpeg2ps$(EXE)",).NES."" THEN  $(RM) jpeg2ps$(EXE);*

jpeg2ps.$(OBJ) :	jpeg2ps.c psimage.h

readjpeg.$(OBJ) :	readjpeg.c psimage.h

asc85ec.$(OBJ) :	asc85ec.c

getopt.$(OBJ) :		getopt.c
