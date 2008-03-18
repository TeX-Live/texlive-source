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

# some global variables
SRCDIR     = ../src
FAS        = base.fas locref.fas idxstyle.fas index.fas version.fas markup.fas
DEFAULTS   = defaults.xdy
DUMP_XINDY = $(SRCDIR)/dump-xindy.lsp
MEMFILE    = xindy.mem
XINDY_RUN  = ./xindy.run -q -M ./base.mem

# Where the modules are found at runtime.
MODULEDIR = $(pkglibdir)/modules

# Where the files xindy.run and xindy.mem should be installed.
kerneldir = $(pkglibdir)


xindy.mem: $(DEFAULTS) $(FAS) base.mem xindy.run
	rm -f $(MEMFILE)
	$(XINDY_RUN) -x '(load "$(DUMP_XINDY)")'
	gzip $(MEMFILE)
	mv $(MEMFILE).gz $(MEMFILE)


$(DEFAULTS):
	@$(RM) $(DEFAULTS)
	@echo "Creating file $(DEFAULTS)..."
	@echo "(setq *load-paths* (list "         > $(DEFAULTS)
	@echo "  (pathname \".\")"               >> $(DEFAULTS)
	@echo "  (pathname \"$(MODULEDIR)\")))"  >> $(DEFAULTS)
	@echo "Finished $(DEFAULTS)."

%.fas : $(SRCDIR)/%.lsp
	$(XINDY_RUN) -c $< -o $@

# *.lib files which should be cleaned
FASLIBS = base.lib locref.lib idxstyle.lib index.lib version.lib markup.lib

kernel_DATA = xindy.mem

kernel_SCRIPTS = xindy.run

CLEANFILES = $(FAS) $(FASLIBS) defaults.xdy base.mem \
	$(kernel_DATA) $(kernel_SCRIPTS)


