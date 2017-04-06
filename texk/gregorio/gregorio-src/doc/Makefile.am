# Copyright (C) 2006-2017 The Gregorio Project (see CONTRIBUTORS.md)
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

# GregorioRef.tex must be the first source file
SRCFILES = GregorioRef.tex Command_Index_gregorio.tex \
		   Command_Index_internal.tex Command_Index_User.tex \
		   Gabc.tex Appendix_Font_Tables.tex GregorioRef.lua factus.gabc
NABCSRCFILES = GregorioNabcRef.tex veni.gabc omnes.gabc

GREGORIO=gregorio-$(FILENAME_VERSION)

.NOTPARALLEL:

# I know these rules look wrong, but they must not depend on anything that
# gets generated or make distcheck will fail.

GregorioRef.pdf: $(SRCFILES)
	$(MAKE) $(AM_MAKEFLAGS) -C ../src $(GREGORIO)
	../src/$(GREGORIO) -o factus.gtex $(<D)/factus.gabc
	TEXINPUTS=$(<D):$(<D)/../tex: LUAINPUTS=$(<D):$(<D)/../tex: \
		 TTFONTS=$(<D)/../fonts: PATH=../src:${PATH} latexmk -f -recorder -pdf \
		 -interaction=nonstopmode -halt-on-error \
		 -pdflatex='lualatex --shell-escape %O %S' \
		 -jobname=GregorioRef $< || rm $@

GregorioNabcRef.pdf: $(NABCSRCFILES)
	$(MAKE) $(AM_MAKEFLAGS) -C ../src $(GREGORIO)
	../src/$(GREGORIO) -o veni.gtex $(<D)/veni.gabc
	../src/$(GREGORIO) -o omnes.gtex $(<D)/omnes.gabc
	TEXINPUTS=$(<D):$(<D)/../tex: LUAINPUTS=$(<D):$(<D)/../tex: \
		 TTFONTS=$(<D)/../fonts: PATH=../src:${PATH} latexmk -recorder -pdf \
		 -interaction=nonstopmode -halt-on-error \
		 -pdflatex='lualatex --shell-escape %O %S' \
		 -jobname=GregorioNabcRef $< || rm $@

doc: GregorioRef.pdf GregorioNabcRef.pdf

pdf-local: doc

clean-doc-intermediate:
	latexmk -quiet -c -f -jobname=GregorioRef GregorioRef.tex
	latexmk -quiet -c -f -jobname=GregorioNabcRef GregorioNabcRef.tex
	rm -rf _minted* *.gtex *.aux *.gaux *.glog

clean-doc: clean-doc-intermediate
	rm -rf GregorioRef.pdf
	rm -rf GregorioNabcRef.pdf

distclean-local: clean-doc-intermediate

maintainer-clean-local: clean-doc

EXTRA_DIST = $(SRCFILES) $(NABCSRCFILES) \
	     GregorioRef.pdf \
	     GregorioNabcRef.pdf \
	     README.md
