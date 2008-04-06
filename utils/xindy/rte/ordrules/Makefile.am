## Use `autoreconf' in top-level directory to recreate all
## autoconf/automake files.
## Or, call `automake Makefile' to create only Makefile.in.

# Makefile for xindy

## Copyright (C) 2004-2005 by Gour.
## Copyright (C) 2008 by Joachim Schrod.
##
## This program is free software; you can redistribute it and/or
## modify it under the terms of the GNU General Public License as
## published by the Free Software Foundation; either version 2 of the
## License, or (at your option) any later version.
##
## This program is distributed in the hope that it will be useful, but
## WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
## General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program in the file COPYING; if not, write to the
## Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
## Boston, MA 02110-1301 USA.

INCLUDES=

ordrulei.c: $(srcdir)/ordrulei.lsp
	$(CLISP) -q -norc -c $(srcdir)/ordrulei.lsp -o ordrulei.fas

ordrulei.o: ordrulei.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -I$(INCLUDES) -c ordrulei.c

ordrules.o: ordrules.c ordrules.h rxsub.h debug.h mkind.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -I$(INCLUDES) -funsigned-char -I. -c ordrules.c

rxsub.o: rxsub.c rxsub.h mkind.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -I. -c rxsub.c

# Make a module
clisp-module: ordrulei.o ordrules.o rxsub.o

EXTRA_DIST = ordrulei.lsp ordrules.c ordrules.h rxsub.c rxsub.h mkind.h debug.h link.sh

CLEANFILES = ordrulei.c ordrulei.fas ordrulei.lib ordrulei.o ordrules.o rxsub.o

DISTCLEANFILES = version.lisp
