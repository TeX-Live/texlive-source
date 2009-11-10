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
install-exec-hook: xindy
	$(MKDIR_P) "$(DESTDIR)$(prefix)/texmf/scripts/xindy"
	-rm -f $(DESTDIR)$(bindir)/xindy $(DESTDIR)$(bindir)/texindy
	$(INSTALL_SCRIPT) xindy $(DESTDIR)$(prefix)/texmf/scripts/xindy/xindy.pl
	$(INSTALL_SCRIPT) $(srcdir)/texindy $(DESTDIR)$(prefix)/texmf/scripts/xindy/texindy.pl
	if test -d "$(DESTDIR)$(bindir)/../../texmf/scripts/xindy"; then \
	  ln -s ../../texmf/scripts/xindy/xindy.pl $(DESTDIR)$(bindir)/xindy; \
	  ln -s ../../texmf/scripts/xindy/texindy.pl $(DESTDIR)$(bindir)/texindy; \
	elif test -d "$(DESTDIR)$(bindir)/../texmf/scripts/xindy"; then \
	  ln -s ../texmf/scripts/xindy/xindy.pl $(DESTDIR)$(bindir)/xindy; \
	  ln -s ../texmf/scripts/xindy/texindy.pl $(DESTDIR)$(bindir)/texindy; \
	else \
	  echo "Error: texmf/scripts/xindy directory not found" >&2; \
	  exit 1; \
	fi
uninstall-hook:
	rm -f $(DESTDIR)$(bindir)/xindy $(DESTDIR)$(bindir)/texindy \
	  $(DESTDIR)$(prefix)/texmf/scripts/xindy/xindy.pl \
	  $(DESTDIR)$(prefix)/texmf/scripts/xindy/texindy.pl
else
    bin_SCRIPTS = xindy
    dist_bin_SCRIPTS = texindy
endif

xindy: xindy.in
	sed -e 's:@libdir\@:$(libdir):' $(srcdir)/xindy.in >xindy

man_MANS = texindy.1 xindy.1

dist_noinst_SCRIPTS = xindy.v2 makeindex4

EXTRA_DIST = $(man_MANS) xindy.in xindy.v2.1 makeindex4.1

CLEANFILES = xindy

if MAINTAINER_MODE
POD_DATE = `ident $< | grep '\$$Id' | sed -e 's/^.*,v [^ ]* //' -e 's/ .*//' -e s:/:-:g`
POD_VERSION = `ident $< | grep '\$$Id' | sed -e 's/^.*,v //' -e 's/ .*//'`
POD_NAME = `basename $@ .1 | tr '[a-z]' '[A-Z]'`
POD2MAN = pod2man

$(srcdir)/xindy.1: $(srcdir)/xindy.in
	$(POD2MAN) --center '$(PACKAGE_NAME)' \
		--name="$(POD_NAME)" \
		--release "Version $(POD_VERSION)" \
		--date "$(POD_DATE)" \
		$< $@

$(srcdir)/texindy.1: $(srcdir)/texindy
	$(POD2MAN) --center '$(PACKAGE_NAME)' \
		--name="$(POD_NAME)" \
		--release "Version $(POD_VERSION)" \
		--date "$(POD_DATE)" \
		$< $@
endif
