# Copyright (C) 2006-2019 The Gregorio Project (see CONTRIBUTORS.md)
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

SUBDIRS = src examples tex contrib fonts doc

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = README.md CHANGELOG.md CONTRIBUTING.md CONTRIBUTORS.md \
			 UPGRADE.md COPYING.md .gregorio-version VersionManager.py \
			 build.sh install.sh install-gtex.sh

MAINTAINERCLEANFILES = gregoriotex.ctan.zip

export TOP_LEVEL_MAKE = true

gregorio:
	make -C src all

fonts: make-fonts
make-fonts:
	make -C fonts fonts

clean-fonts:
	make -C fonts clean-fonts

doc: pdf
clean-pdf: clean-doc
clean-doc:
	make -C doc clean-doc

gregoriotex.tds.zip: dist install-gtex.sh
	./install-gtex.sh tds

ctan: dist gregoriotex.tds.zip build-ctan.sh
	./build-ctan.sh
