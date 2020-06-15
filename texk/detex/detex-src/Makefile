# Copyright (c) 1986-2007 Purdue University
# All rights reserved.
# 
# Developed by:  Daniel Trinkle
#                Department of Computer Science, Purdue University
#                http://www.cs.purdue.edu/
# 
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal with the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
# 
# o Redistributions of source code must retain the above copyright
#   notice, this list of conditions and the following disclaimers.
# 
# o Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimers in the
#   documentation and/or other materials provided with the distribution.
# 
# o Neither the names of Daniel Trinkle, Purdue University, nor the
#   names of its contributors may be used to endorse or promote products
#   derived from this Software without specific prior written
#   permission.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR
# ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
# CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
# WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS WITH THE SOFTWARE.
#
#
#	Makefile for detex and delatex
#
# Detex is a program to remove TeX and LaTeX constructs from text source.

UNAME_S := $(shell uname -s)

# Installation directory
#
DESTDIR	= /usr/local/bin

# Specify you favorite compiler
#
#CC	= gcc

# Compile time flags, just uncomment the necessary lines
# Some say GNU make does not correctly handle += -- you may have to use :=
#
DEFS	=
#
# Add -traditional for GNU cc on ISC 386/ix system and possibly others
# (reported by pinard@iro.umontreal.ca)
#
#DEFS	+= ${DEFS} -traditional
#
# Add -DHAVE_STRING_H for the SysV string manipulation routines
#
#DEFS	+= ${DEFS} -DHAVE_STRING_H
#
# Add -DMAXPATHLEN=<length> if it is not defined in /usr/include/sys/param.h
#
#DEFS	+= ${DEFS} -DMAXPATHLEN=1024
#
# Add -DNO_MALLOC_DECL if your system does not like the malloc() declaration
# in detex.l (reported by pinard@iro.umontreal.ca)
#
#DEFS	+= ${DEFS} -DNO_MALLOC_DECL
#
CFLAGS	= -O -DVERSION=\"${VERSION}\" ${DEFS} -Wall

# Use your favorite lexical scanner
#
#LEX	= lex
LEX	= flex

#LFLAGS	= -8 -C

# scanner library
#
LEXLIB	= -lfl
ifeq ($(UNAME_S),Darwin)
	LEXLIB	= -ll
endif

LPR	= lpr -p

# Program names
#
PROGS	= detex delatex

# Header files
#
HDR	= detex.h

# Sources
#
SRC	= detex.l

# Objects for various programs
#
D_OBJ	= detex.o

VERSION = 2.8.7

all:	${PROGS}

detex: ${D_OBJ}
	${CC} ${CFLAGS} -o $@ ${D_OBJ} ${LEXLIB}

delatex: detex
	cp detex delatex

detex.c: detex.l
	${LEX} ${LFLAGS} detex.l
	mv lex.yy.c detex.c

man-page:
	troff -man detex.1

# If you want detex available as delatex, uncomment the last two lines of
# this target
install: all
	rm -f ${DESTDIR}/detex
	install -c -m 775 -g staff -s detex ${DESTDIR}
	install detex.1 /usr/local/share/man/man1
#	rm -f ${DESTDIR}/delatex
#	ln ${DESTDIR}/detex ${DESTDIR}/delatex

uninstall:
	rm -f ${DESTDIR}/detex
	rm -f ${DESTDIR}/delatex
	rm -f /usr/local/share/man/man1/detex.*

clean:
	-rm -f a.out core *.s *.o ERRS errs .,* .emacs_[0-9]*
	-rm -f ${PROGS} xxx.l lex.yy.c detex.c
	-rm -f *.tar.bz2

print:	${HDR} ${SRC}
	${LPR} Makefile ${HDR} ${SRC}

test: all
	./test.pl

run: delatex
	./delatex in > out.txt

package: clean detex.c
	tar cjfv opendetex-${VERSION}.tar.bz2 --exclude='*.o' ChangeLog COPYRIGHT detex.* INSTALL Makefile README

# Dependencies
#
detex.c: detex.h
detex.c: detex.l

