## Use `autoreconf' in top-level directory to recreate all
## autoconf/automake files.
## Or, call `automake Makefile' to create only Makefile.in.

## Copyright (C) 2004-2005 by Gour.
## Copyright (C) 2006-2007 by Jörg Sommer.
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

clisp_src = clisp-2.43

clisp_builddir = $(shell pwd)/clisp-build-dir

if EXT_CLISP
    export CLISP_DIR = @CLISP_DIR@
    export CLISP_PATH = @CLISP_PATH@
    CLISP_PREREQ =
else
    export CLISP_DIR = $(clisp_builddir)
    export CLISP_PATH = $(CLISP_DIR)/clisp
    CLISP_PREREQ = $(clisp_link) $(fulldir)
endif

export CLISP_LINKKIT = $(CLISP_DIR)/linkkit
fulldir = $(CLISP_DIR)/full
clisp_link = $(CLISP_DIR)/clisp-link

binariesdir = ../binaries

#clisp_modules=--with-export-syscalls --with-dynamic-ffi --ignore-absence-of-libsigsegv
clisp_modules := --with-dynamic-ffi --ignore-absence-of-libsigsegv

# testing for MacOS X platform to circumvent faulty 'readline'
# provided by Maarten Sneep <maarten.sneep@xs4all.nl>
# FIXME: GNU make ifeq/endif clauses do not work in Automake files.
# Either determine that case during configure or make them work.
#ifeq ($(shell uname -s), Darwin)
#	clisp_modules += --without-readline
#endif

all: xindy-build-dir

RM_R = rm -rf

if !EXT_CLISP
$(clisp_builddir): $(clisp_src)
	cd $(clisp_src) && ./configure $(clisp_builddir) $(clisp_modules)
	cd $(clisp_builddir) && ./makemake $(clisp_modules) > Makefile

$(CLISP_PREREQ): $(clisp_builddir)
	$(MAKE) -C $(clisp_builddir)

# Target names that are easy to type, to build CLISP alone during development.
clisp-configure: $(clisp_builddir)
clisp: $(CLISP_PREREQ)

check:
	$(MAKE) -C $(clisp_builddir) check || $(MAKE) -C $(clisp_builddir) check
endif

xindy-build-dir: $(CLISP_PREREQ)
	$(RM_R) xindy-build-dir
	$(MAKE) -C ordrules ordrulei.c
	$(clisp_link) add-module-set ordrules $(fulldir) $@
	cp $@/lisp.run $(binariesdir)/xindy.run
	cp $@/lispinit.mem $(binariesdir)/base.mem

clean-local:
	$(RM_R) xindy-build-dir
	$(MAKE) -C ordrules clean
if !EXT_CLISP
	if [ -e $(clisp_builddir) ]; then $(MAKE) -C $(clisp_builddir) clean; fi
endif

# remove some extra files to match pristine clisp source
distclean-local: clean
	$(MAKE) -C ordrules distclean
if !EXT_CLISP
	$(RM_R) $(clisp_builddir)
endif

# dist rules for clisp are handled via 'darcs dist', so here we include
# only the rest of the 'rte' package + modified clisp's configure script!!

EXTRA_DIST = NEWS README configure ordrules $(clisp_src)/configure
