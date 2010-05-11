## Use `autoreconf' in top-level directory to recreate all
## autoconf/automake files.
## Or, call `automake Makefile' to create only Makefile.in.

## Copyright (C) 2004-2005 by Gour.
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

bin_PROGRAMS = tex2xindy

tex2xindy_SOURCES = tex2xindy.l

#tex2xindy_LDADD = @LEXLIB@

man_MANS = tex2xindy.1

EXTRA_DIST = $(man_MANS)

if MAINTAINER_MODE
POD_DATE = `ident $< | grep '\$$Id' | sed -e 's/^.*,v [^ ]* //' -e 's/ .*//' -e s:/:-:g`
POD_VERSION = `ident $< | grep '\$$Id' | sed -e 's/^.*,v //' -e 's/ .*//'`
POD_NAME = `basename $@ .1 | tr '[a-z]' '[A-Z]'`
POD2MAN = pod2man

$(srcdir)/tex2xindy.1: $(srcdir)/tex2xindy.l
	$(POD2MAN) --center '$(PACKAGE_NAME)' \
		--name="$(POD_NAME)" \
		--release "Version $(POD_VERSION)" \
		--date "$(POD_DATE)" \
		$< $@
endif
