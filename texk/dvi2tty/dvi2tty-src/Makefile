# Makefile for dvi2tty and disdvi       23/01/89   M.J.E. Mol
#
# For BSD Unix use the following CFLAGS definition
# CFLAGS = -Dstrchr=index
#
# This Makefile does not work for MSDOS. Make your 
# own one, or compile by hand.
#
CC = gcc

CFLAGS = -Wall -O2
# CFLAGS = -Wall -O2 -fomit-frame-pointer
LDFLAGS = -s

prefix = ${DESTDIR}/usr
mandir = $(prefix)/share/man/man1
bindir = $(prefix)/bin


all:	dvi2tty disdvi

dvi2tty:dvi2tty.o dvistuff.o
	$(CC) $(LFLAGS) -o dvi2tty dvi2tty.o dvistuff.o

disdvi:disdvi.o
	$(CC) $(LFLAGS) -o disdvi disdvi.o

dvi2tty.o: dvi2tty.c dvi2tty.h

dvistuff.o: dvistuff.c dvi2tty.h commands.h

disdvi.o: disdvi.c commands.h

clean:
	rm -f dvi2tty disdvi *.o

install:	all
	install dvi2tty disdvi $(bindir)
	install dvi2tty.1 $(mandir)
	install disdvi.1 $(mandir)
