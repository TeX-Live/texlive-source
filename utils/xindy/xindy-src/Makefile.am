## Use `autoreconf' in top-level directory to recreate all
## autoconf/automake files.
## Or, call `automake Makefile' to create only Makefile.in.

# Makefile for xindy

## Copyright (C) 2004-2005 by Gour.
## Copyright (C) 2008,2009 by Joachim Schrod.
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

## Global options

ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src tex2xindy modules user-commands

# do we build make-rules?
if BUILDRULES
  SUBDIRS += make-rules
endif

# do we build docs?
if BUILDDOCS
  SUBDIRS += doc
endif

EXTRA_DIST = ChangeLog.Gour VERSION TODO.autoconf

versiondir = ${pkglibdir}
version_DATA = VERSION


distclean-local:
	rm -rf autom4te.cache

