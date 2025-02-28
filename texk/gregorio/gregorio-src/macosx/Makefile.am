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

MACOSX_TOP := $(shell pwd)
MACOSX_BUILD := $(MACOSX_TOP)/build
MACOSX_RESOURCES := $(MACOSX_TOP)/Resources
HTMLPREFIX := "https://github.com/gregorio-project/gregorio/blob/v"$(VERSION)"/"

$(MACOSX_BUILD) :
	mkdir -p $(MACOSX_BUILD)

Gregorio : $(MACOSX_BUILD)
	mkdir -p $(MACOSX_BUILD)/Gregorio
	cp $(TOP)/*.md $(MACOSX_BUILD)/Gregorio

$(MACOSX_RESOURCES) :
	mkdir -p $(MACOSX_RESOURCES)

Resources/COPYING.html : $(MACOSX_RESOURCES)
	pandoc -s $(TOP)/COPYING.md -f markdown -t html --metadata title="COPYING" -o $(MACOSX_RESOURCES)/COPYING.html
	sed -i -E "s|(href=.)([A-Z]+.md.)|\1$(HTMLPREFIX)\2|g" $(MACOSX_RESOURCES)/COPYING.html
	sed -i -E "s|\(C\)|\&copy;|g" $(MACOSX_RESOURCES)/COPYING.html
	sed -i -E "s|©|\&copy;|g" $(MACOSX_RESOURCES)/COPYING.html
	sed -i -E "s|--|\&#8212;|g" $(MACOSX_RESOURCES)/COPYING.html
	sed -i -E "s|“|\&#8220;|g" $(MACOSX_RESOURCES)/COPYING.html
	sed -i -E "s|”|\&#8221;|g" $(MACOSX_RESOURCES)/COPYING.html

Resources/README.html : $(MACOSX_RESOURCES)
	pandoc -s $(TOP)/README.md -f markdown -t html --metadata title="README" -o $(MACOSX_RESOURCES)/README.html
	sed -i -E "s|(href=.)([A-Z]+.md.)|\1$(HTMLPREFIX)\2|g" $(MACOSX_RESOURCES)/README.html
	sed -i -E "s|\(C\)|\&copy;|g" $(MACOSX_RESOURCES)/README.html
	sed -i -E "s|©|\&copy;|g" $(MACOSX_RESOURCES)/README.html
	sed -i -E "s|--|\&#8212;|g" $(MACOSX_RESOURCES)/README.html
	sed -i -E "s|“|\&#8220;|g" $(MACOSX_RESOURCES)/README.html
	sed -i -E "s|”|\&#8221;|g" $(MACOSX_RESOURCES)/README.html

resources : Resources/COPYING.html Resources/README.html

examples : Gregorio
	mkdir -p $(MACOSX_BUILD)/Gregorio/examples
	cp $(TOP)/examples/*.tex $(MACOSX_BUILD)/Gregorio/examples/
	cp $(TOP)/examples/*.gabc $(MACOSX_BUILD)/Gregorio/examples/

mac-doc : Gregorio
	mkdir -p $(MACOSX_BUILD)/Gregorio/doc
	cp $(TOP)/doc/*.tex $(MACOSX_BUILD)/Gregorio/doc/
	cp $(TOP)/doc/*.lua $(MACOSX_BUILD)/Gregorio/doc/
	cp $(TOP)/doc/*.gabc $(MACOSX_BUILD)/Gregorio/doc/
	cp $(TOP)/doc/*.pdf $(MACOSX_BUILD)/Gregorio/doc/
	cp $(TOP)/doc/doc_README.md $(MACOSX_BUILD)/Gregorio/doc/
	cp -r $(TOP)/contrib/ $(MACOSX_BUILD)/Gregorio/contrib
	find $(MACOSX_BUILD)/Gregorio/contrib -name 'Makefile*' -delete

payload : $(MACOSX_BUILD) mac-doc examples
	$(MAKE) -C .. DESTDIR=$(MACOSX_BUILD) install
	AUTO_UNINSTALL=true $(TOP)/install-gtex.sh dir:$(MACOSX_BUILD)/tmp/gregorio
	rm $(MACOSX_BUILD)/tmp/gregorio/fonts/source/gregoriotex/gregorio-base.sfd
	rm $(MACOSX_BUILD)/tmp/gregorio/fonts/source/gregoriotex/granapadano-base.sfd

%.pkg : %.pkgproj payload resources
	packagesbuild $<
	mv $(MACOSX_BUILD)/$@ $(MACOSX_TOP)

dist-macosx : Gregorio.pkg Uninstall-Gregorio.pkg
	mv Gregorio.pkg Gregorio-$(VERSION).pkg
	mv Uninstall-Gregorio.pkg Uninstall-Gregorio-$(VERSION).pkg

clean-local :
	rm -rf $(MACOSX_BUILD)
	rm -rf $(MACOSX_RESOURCES)
	rm -f *.pkg
