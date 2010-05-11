## Use `autoreconf' in top-level directory to recreate all
## autoconf/automake files.
## Or, call `automake Makefile' to create only Makefile.in.

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

# Where the modules are found at runtime.
MODULEDIR = $(pkglibdir)/modules


# Automake has no CLISP support; moreover xindy.mem is not a binary
# to be stripped upon 'make install-strip'.  We define it as DATA.
mem_DATA = xindy.mem

# Tell the dist target about our sources, define the object files for
# our own rule and the .lib files for the clean definition. Don't use
# gmake-isms.
FASSRC                 = base.lsp ordrules.lsp locref.lsp idxstyle.lsp \
			 index.lsp version.lsp markup.lsp
FAS                    = base.fas ordrules.fas locref.fas idxstyle.fas \
			 index.fas version.fas markup.fas
FASLIBS 	       = base.lib ordrules.lib locref.lib idxstyle.lib \
			 index.lib version.lib markup.lib

DEFAULTS   = defaults.xdy
DUMP_XINDY = dump-xindy.lsp
MEMFILE    = xindy.mem
CLISP      = @CLISP@ -q -E iso-8859-1

xindy.mem: $(DEFAULTS) $(FAS) $(DUMP_XINDY)
	rm -f $(MEMFILE)
	$(CLISP) $(srcdir)/$(DUMP_XINDY)
	if test "$(host_os)" != "mingw32"; then \
	  gzip -9n $@; \
	  mv $@.gz $@; \
	fi

$(DEFAULTS): $(DEFAULTS).in Makefile
	sed 's|@MODULEDIR[@]|$(MODULEDIR)|g' <$(srcdir)/$(DEFAULTS).in >$(DEFAULTS)

EXTRA_DIST = $(DEFAULTS).in $(FASSRC) dump-xindy.lsp

SUFFIXES = .fas .lsp
.lsp.fas:
	$(CLISP) -c $< -o $@


CLEANFILES = $(FAS) $(FASLIBS) $(DEFAULTS) xindy.mem

# Dependencies required for parallel make.
markup.fas: index.fas version.fas
index.fas: idxstyle.fas ordrules.fas
idxstyle.fas: locref.fas
locref.fas: base.fas

