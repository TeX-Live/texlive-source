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
SRCDIR     = $(top_srcdir)/src
FAS        = base.fas locref.fas idxstyle.fas index.fas version.fas markup.fas
DUMP_XINDY = $(SRCDIR)/dump-xindy.lsp
RUN_XINDY  = ./$(XINDY_RUN) -q -norc -M ./base.mem

if TETEX_BUILD
    kerneldir = $(bindir)
else
# Where the files xindy.run and xindy.mem should be installed.
    kerneldir = $(pkglibdir)
endif

xindy.mem: defaults.xdy $(FAS) base.mem $(XINDY_RUN)
	rm -f $@
	$(RUN_XINDY) -x '(load "$(DUMP_XINDY)")'
	if test "$(host_os)" != "mingw32"; then gzip -9n $@; mv $@.gz $@; fi

defaults.xdy:
	@$(RM) $@
	@echo "Creating file $@ ..."
	@echo "(setq *load-paths* (list "         > $@
	@echo "  (pathname \".\")"               >> $@
	@echo "  (pathname \"$(pkgdatadir)\")))" >> $@
	@echo "Finished $@."

%.fas: $(SRCDIR)/%.lsp
	$(RUN_XINDY) -c $< -o $@

# *.lib files which should be cleaned
FASLIBS = base.lib locref.lib idxstyle.lib index.lib version.lib markup.lib

kernel_DATA = xindy.mem

kernel_SCRIPTS = $(XINDY_RUN)

EXTRA_SCRIPTS = xindy.run xindy-lisp.exe

CLEANFILES = $(FAS) $(FASLIBS) defaults.xdy base.mem xindy.mem $(XINDY_RUN)

# Serialize
.NOTPARALLEL:
