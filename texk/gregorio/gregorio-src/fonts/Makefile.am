## This makefile is made to build to fonts from the sfd, it won't be useful for
## the average user.

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

GREGORIO_FONTS = fonts

TTF_GRECILIAE = greciliae.ttf greciliae-hollow.ttf greciliae-hole.ttf
TTF_GRECILIAE_OP = greciliae-op.ttf greciliae-op-hollow.ttf greciliae-op-hole.ttf
TTF_GREGORIO = gregorio.ttf gregorio-hollow.ttf gregorio-hole.ttf
TTF_GREGORIO_OP = gregorio-op.ttf gregorio-op-hollow.ttf gregorio-op-hole.ttf
TTF_GRANAPADANO = granapadano.ttf granapadano-hollow.ttf granapadano-hole.ttf
TTF_GRANAPADANO_OP = granapadano-op.ttf granapadano-op-hollow.ttf \
	granapadano-op-hole.ttf
TTF_EXTRA = greextra.ttf gregall.ttf gresgmodern.ttf grelaon.ttf
TTF_ALL = $(TTF_GRECILIAE) $(TTF_GRECILIAE_OP) \
	$(TTF_GREGORIO) $(TTF_GREGORIO_OP) \
	$(TTF_GRANAPADANO) $(TTF_GRANAPADANO_OP) $(TTF_EXTRA)
SOURCEFILES_GRECILIAE = greciliae-base.sfd
SOURCEFILES_GREGORIO = gregorio-base.sfd
SOURCEFILES_GRANAPADANO = granapadano-base.sfd
SOURCEFILES_EXTRA = greextra.sfd gregall.sfd gresgmodern.sfd grelaon.sfd
SOURCEFILES_TOOLS = squarize.py stemsschemas.py convertsfdtottf.py fonts_README.md \
	simplify.py
SOURCEFILES_ALL = $(SOURCEFILES_GREGORIO) $(SOURCEFILES_GRANAPADANO) \
	$(SOURCEFILES_GRECILIAE)

EXTRA_DIST = $(TTF_GRECILIAE) $(TTF_GRECILIAE_OP) $(TTF_EXTRA) \
	$(SOURCEFILES_GRECILIAE) $(SOURCEFILES_EXTRA) $(SOURCEFILES_TOOLS)

MAINTAINERCLEANFILES = supp_fonts-$(FILENAME_VERSION).zip *.pyc $(TTF_ALL)

FONTFORGE = fontforge
RM = rm

%.ttf: %-base.sfd squarize.py stemsschemas.py simplify.py %.json
	$(FONTFORGE) -script squarize.py $< -o $@ -c $*.json
	$(FONTFORGE) -script simplify.py $@

%-hollow.ttf: %-base.sfd squarize.py stemsschemas.py simplify.py %.json
	$(FONTFORGE) -script squarize.py $< -o $@ -v hollow -c $*.json
	$(FONTFORGE) -script simplify.py $@

%-hole.ttf: %-base.sfd squarize.py stemsschemas.py simplify.py %.json
	$(FONTFORGE) -script squarize.py $< -o $@ -v hole -c $*.json
	$(FONTFORGE) -script simplify.py $@

%-op.ttf: %-base.sfd squarize.py stemsschemas.py simplify.py %.json
	$(FONTFORGE) -script squarize.py $< -o $@ -s op -c $*.json
	$(FONTFORGE) -script simplify.py $@

%-op-hollow.ttf: %-base.sfd squarize.py stemsschemas.py simplify.py %.json
	$(FONTFORGE) -script squarize.py $< -o $@ -s op -v hollow -c $*.json
	$(FONTFORGE) -script simplify.py $@

%-op-hole.ttf: %-base.sfd squarize.py stemsschemas.py simplify.py %.json
	$(FONTFORGE) -script squarize.py $< -o $@ -s op -v hole -c $*.json
	$(FONTFORGE) -script simplify.py $@

%.ttf: convertsfdtottf.py %.sfd
	$(FONTFORGE) -script $< $*.sfd

clean-fonts:
	$(RM) -f $(TTF_GRECILIAE) $(TTF_GRECILIAE_OP) $(TTF_GREGORIO) \
		$(TTF_GREGORIO_OP) $(TTF_GRANAPADANO) $(TTF_GRANAPADANO_OP)

greciliae: $(TTF_GRECILIAE)

greciliae-op: $(TTF_GRECILIAE_OP)

greciliae-all: greciliae greciliae-op

gregorio: $(TTF_GREGORIO)

gregorio-op: $(TTF_GREGORIO_OP)

gregorio-all: gregorio gregorio-op

granapadano: $(TTF_GRANAPADANO)

granapadano-op: $(TTF_GRANAPADANO_OP)

granapadano-all: granapadano granapadano-op

extra: $(TTF_EXTRA)

install-data-local:
	@$(TOP_LEVEL_MAKE) echo 'The Makefile in the fonts directory is no longer used for installing'
	@$(TOP_LEVEL_MAKE) echo 'GregorioTeX.  Please use install-gtex.sh from the parent directory'
	@$(TOP_LEVEL_MAKE) echo 'to install GregorioTeX.'
	@$(TOP_LEVEL_MAKE) echo
	@$(TOP_LEVEL_MAKE) echo 'If you were using "make install" before, now use'
	@$(TOP_LEVEL_MAKE) echo '    ./install-gtex.sh system'
	@$(TOP_LEVEL_MAKE) echo
	@$(TOP_LEVEL_MAKE) echo 'If you were using "make localinstall" before, now use'
	@$(TOP_LEVEL_MAKE) echo '    ./install-gtex.sh user'

really-all-fonts: greciliae-all gregorio-all granapadano-all extra

fonts: greciliae-all extra

all: $(GREGORIO_FONTS)

supp-fonts-dist: gregorio-all granapadano-all
	zip supp_fonts-$(FILENAME_VERSION).zip $(TTF_GREGORIO) $(TTF_GREGORIO_OP) \
		$(TTF_GRANAPADANO) $(TTF_GRANDAPADANO_OP) \
		$(SOURCEFILES_GREGORIO) $(SOURCEFILES_GRANAPADANO) \
		install_supp_fonts.lua README_supp.md
