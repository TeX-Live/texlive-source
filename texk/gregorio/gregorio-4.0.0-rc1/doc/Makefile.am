# Copyright (C) 2006-2015 The Gregorio Project (see CONTRIBUTORS.md)
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
NABCSRCFILES = GregorioNabcRef.tex veni.gabc

.NOTPARALLEL:

# I know these rules look wrong, but they must not depend on anything that
# gets generated or make distcheck will fail.

GregorioRef-@FILENAME_VERSION@.pdf: $(SRCFILES)
	$(MAKE) $(AM_MAKEFLAGS) -C ../src gregorio
	../src/gregorio -o factus.gtex $(<D)/factus.gabc
	TEXINPUTS=$(<D):$(<D)/../tex: LUAINPUTS=$(<D):$(<D)/../tex: \
		 TTFONTS=$(<D)/../fonts: PATH=../src:${PATH} latexmk -recorder -pdf \
		 -pdflatex='lualatex --shell-escape %O %S' \
		 -jobname=GregorioRef-@FILENAME_VERSION@ $<

GregorioNabcRef-@FILENAME_VERSION@.pdf: $(NABCSRCFILES)
	$(MAKE) $(AM_MAKEFLAGS) -C ../src gregorio
	../src/gregorio -o veni.gtex $(<D)/veni.gabc
	TEXINPUTS=$(<D):$(<D)/../tex: LUAINPUTS=$(<D):$(<D)/../tex: \
		 TTFONTS=$(<D)/../fonts: PATH=../src:${PATH} latexmk -recorder -pdf \
		 -pdflatex='lualatex --shell-escape %O %S' \
		 -jobname=GregorioNabcRef-@FILENAME_VERSION@ $<

doc: GregorioRef-$(FILENAME_VERSION).pdf GregorioNabcRef-$(FILENAME_VERSION).pdf

pdf-local: doc

distclean-local:
	latexmk -quiet -c -f -jobname=GregorioRef-@FILENAME_VERSION@ GregorioRef.tex
	latexmk -quiet -c -f -jobname=GregorioNabcRef-@FILENAME_VERSION@ GregorioNabcRef.tex
	rm -rf _minted* factus.gtex veni.gtex

EXTRA_DIST = $(SRCFILES) $(NABCSRCFILES) \
	     GregorioRef-@FILENAME_VERSION@.pdf \
	     GregorioNabcRef-@FILENAME_VERSION@.pdf
MAINTAINERCLEANFILES = GregorioRef-@FILENAME_VERSION@.pdf \
		       GregorioNabcRef-@FILENAME_VERSION@.pdf
