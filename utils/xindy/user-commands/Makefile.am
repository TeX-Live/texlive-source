## Use `autoreconf' in top-level directory to recreate all
## autoconf/automake files.
## Or, call `automake Makefile' to create only Makefile.in.

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

bin_SCRIPTS = xindy xindy.v2

xindy: xindy.in
	sed -e 's:@libdir\@:$(libdir):' xindy.in >xindy

xindy.v2: xindy.v2.in
	sed -e 's:@XINDY_PERL\@:/usr/bin/perl:' xindy.v2.in >xindy.v2

dist_bin_SCRIPTS = texindy

man_MANS = texindy.1 xindy.1 xindy.v2.1

EXTRA_DIST = $(man_MANS)

CLEANFILES = xindy xindy.v2

