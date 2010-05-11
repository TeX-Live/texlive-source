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

SUBDIRS = ordrules
EXTRA_DIST = NEWS README

ORDRULES_FILES = ordrules.c ordrules.h rxsub.c rxsub.h mkind.h debug.h link.sh
binariesdir = ../binaries

all-local: $(binariesdir)/$(XINDY_RUN) $(binariesdir)/base.mem

# Serialize to allow parallel builds
#
$(binariesdir)/base.mem: $(binariesdir)/$(XINDY_RUN)
	cp xindy-build-dir/lispinit.mem $(binariesdir)/base.mem
# The clisp-link command 'add-module-set' of Versions <=2.47
# has been replaced by 'add' in Version 2.48 (different args!).
$(binariesdir)/$(XINDY_RUN):
	rm -rf xindy-build-dir
	test -f ordrules/link.sh || for i in $(ORDRULES_FILES); do cp -p $(top_srcdir)/rte/ordrules/$$i ordrules/$$i; done
	$(MAKE) -C ordrules ordrulei.c
	env CLISP_LINKKIT='$(CLISP_LIB)/linkkit' MAKE="$(MAKE)" \
	sh '$(CLISP_LIB)/clisp-link' add-module-set ordrules '$(CLISP_LIB)/full' xindy-build-dir || \
	sh '$(CLISP_LIB)/clisp-link' add '$(CLISP_LIB)/full' xindy-build-dir ordrules
	$(INSTALL_STRIP_PROGRAM) xindy-build-dir/$(LISP_RUN) $(binariesdir)/$(XINDY_RUN)

clean-local:
	rm -rf xindy-build-dir
	test "$(builddir)" = "$(srcdir)" || for i in $(ORDRULES_FILES); do rm -f ordrules/$$i; done
