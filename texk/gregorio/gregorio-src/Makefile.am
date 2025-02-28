# Copyright (C) 2006-2025 The Gregorio Project (see CONTRIBUTORS.md)
#
# This file is part of Gregorio.
#
# Gregorio is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Gregorio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Gregorio.  If not, see <http://www.gnu.org/licenses/>.

SUBDIRS = src examples tex contrib fonts doc macosx

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = README.md CHANGELOG.md CONTRIBUTING.md CONTRIBUTORS.md \
			 UPGRADE.md COPYING.md .gregorio-version VersionManager.py \
			 build.sh install.sh install-gtex.sh

MAINTAINERCLEANFILES = gregoriotex.ctan.zip

VERSION := $(shell ./VersionManager.py --get-current)
TOP := $(shell pwd)
export TOP

export TOP_LEVEL_MAKE = true

gregorio:
	$(MAKE) -C src all

fonts: make-fonts
make-fonts:
	$(MAKE) -C fonts fonts

clean-fonts:
	$(MAKE) -C fonts clean-fonts

doc: pdf
clean-pdf: clean-doc
clean-doc:
	$(MAKE) -C doc clean-doc

gregoriotex.tds.zip: dist-zip install-gtex.sh
	./install-gtex.sh tds

ctan: gregoriotex.tds.zip build-ctan.sh
	./build-ctan.sh

dist-hook:
	rm -rf $(distdir)/windows

if MACOSX
dist-macosx: gregorio doc fonts
	$(MAKE) -C macosx dist-macosx
else
dist-macosx:
	@echo "macOS packages can only be built on Mac systems."
endif

clean-macosx:
	$(MAKE) -C macosx clean-local
