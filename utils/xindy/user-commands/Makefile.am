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

if TETEX_BUILD
install-exec-hook: xindy xindy.v2
	$(INSTALL_SCRIPT) xindy $(prefix)/texmf/scripts/xindy/
	$(LN_S) ../../texmf/scripts/xindy/xindy $(bindir)/xindy
	$(INSTALL_SCRIPT) xindy.v2 $(prefix)/texmf/scripts/xindy/
	$(LN_S) ../../texmf/scripts/xindy/xindy.v2 $(bindir)/xindy.v2
	$(INSTALL_SCRIPT) $(srcdir)/texindy $(prefix)/texmf/scripts/xindy/
	$(LN_S) ../../texmf/scripts/xindy/texindy $(bindir)/texindy
else
    bin_SCRIPTS = xindy xindy.v2
    dist_bin_SCRIPTS = texindy
endif

xindy: xindy.in
	sed -e 's:@libdir\@:$(libdir):' $(srcdir)/xindy.in >xindy

xindy.v2: xindy.v2.in
	sed -e 's:@XINDY_PERL\@:/usr/bin/perl:' $(srcdir)/xindy.v2.in >xindy.v2

$(srcdir)/xindy.1: $(srcdir)/xindy.in
	pod2man --verbose -n XINDY -r "Version 1.8" -c "xindy" $< $@

$(srcdir)/texindy.1: $(srcdir)/texindy
	pod2man --verbose -n TEXINDY -r "Version 1.5" -c "xindy" $< $@

man_MANS = texindy.1 xindy.1 xindy.v2.1

EXTRA_DIST = $(man_MANS)

CLEANFILES = xindy xindy.v2

