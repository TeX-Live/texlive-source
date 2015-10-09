#!/usr/bin/env bash

HERE=`pwd`
OUTPUT=$HERE/system-setup.log

echo "Gregorio Unix Setup Diagnostic Tool"
echo "(C) 2015 The Gregorio Project."
echo ""
echo "Gregorio is free software: you can redistribute it and/or modify"
echo "it under the terms of the GNU General Public License as published by"
echo "the Free Software Foundation, either version 3 of the License, or"
echo "(at your option) any later version."
echo ""
echo "This program is distributed in the hope that it will be useful,"
echo "but WITHOUT ANY WARRANTY; without even the implied warranty of"
echo "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the"
echo "GNU General Public License for more details."
echo ""
echo "You should have received a copy of the GNU General Public License"
echo "along with this program.  If not, see http://www.gnu.org/licenses/."
echo ""
echo "Creating system-setup.log..."

echo "###	Gregorio Unix Setup Results" > $OUTPUT
echo "####	Created: `date`" >> $OUTPUT
echo "-----------------------------------------------------------------------------" >> $OUTPUT
echo "" >> $OUTPUT
echo "" >> $OUTPUT

echo "###	OS Version" >> $OUTPUT
uname -v >> $OUTPUT
echo "" >> $OUTPUT
echo "" >> $OUTPUT
echo "-----------------------------------------------------------------------------" >> $OUTPUT
echo "" >> $OUTPUT
echo "" >> $OUTPUT

echo "###	LuaTeX Setup" >> $OUTPUT
echo "####	Version" >> $OUTPUT
echo "" >> $OUTPUT
luatex -v >> $OUTPUT 2>&1
echo "" >> $OUTPUT
echo "####	Location" >> $OUTPUT
echo "" >> $OUTPUT
which -a luatex >> $OUTPUT 2>&1
echo "" >> $OUTPUT
echo "" >> $OUTPUT
echo "#### 	TEXMFLOCAL" >> $OUTPUT
echo "" >> $OUTPUT
kpsewhich --var-value TEXMFLOCAL >> $OUTPUT 2>&1
echo "" >> $OUTPUT
echo "#### TEXINPUTS.lualatex" >> $OUTPUT
kpsewhich --var-value=TEXINPUTS.lualatex >> $OUTPUT 2>&1
echo "" >> $OUTPUT
echo "#### LUAINPUTS.lualatex" >> $OUTPUT
kpsewhich --var-value=LUAINPUTS.lualatex >> $OUTPUT 2>&1
echo "" >> $OUTPUT
echo "#### shell_escape" >> $OUTPUT
kpsewhich --var-value=shell_escape >> $OUTPUT 2>&1
echo "" >> $OUTPUT
echo "#### Shell Escape Commands" >> $OUTPUT
kpsewhich --var-value=shell_escape_commands >> $OUTPUT 2>&1
echo "" >> $OUTPUT
echo "#### openout_any" >> $OUTPUT
kpsewhich --var-value=openout_any >> $OUTPUT 2>&1
echo "" >> $OUTPUT
echo "#### openin_any" >> $OUTPUT
kpsewhich --var-value=openin_any >> $OUTPUT 2>&1
echo "" >> $OUTPUT
echo "" >> $OUTPUT
echo "-----------------------------------------------------------------------------" >> $OUTPUT
echo "" >> $OUTPUT
echo "" >> $OUTPUT

echo "###	Gregorio Setup" >> $OUTPUT
echo "####	Version" >> $OUTPUT
echo "" >> $OUTPUT
gregorio -V >> $OUTPUT 2>&1
echo "" >> $OUTPUT
echo "#### 	Location" >> $OUTPUT
echo "" >> $OUTPUT
which -a gregorio >> $OUTPUT 2>&1
echo "" >> $OUTPUT
echo "####	GregorioTeX Locations" >> $OUTPUT
echo "" >> $OUTPUT

files="gregoriotex.sty
gregoriosyms.sty
gregoriotex.tex
gregoriotex-main.tex
gregoriotex-chars.tex
gregoriotex-ictus.tex
gregoriotex-signs.tex
gregoriotex-signs.lua
gregoriotex-symbols.tex
gregoriotex-spaces.tex
gregoriotex-syllable.tex
gregoriotex-nabc.tex
gregoriotex-nabc.lua
gregoriotex.lua
gsp-default.tex
gregorio-vowels.dat
greciliae.ttf
greciliae-op.ttf
greextra.ttf
gregorio.ttf
gregorio-op.ttf
gresym.ttf
parmesan.ttf
parmesan-op.ttf
gregall.ttf
gregsmodern.ttf"

for f in $files
do
	echo "##### $f" >> $OUTPUT
	kpsewhich -all $f >> $OUTPUT 2>&1
done

echo "" >> $OUTPUT
echo "####	kpsewhich --all -engine luatex -progname lualatex gregoriotex.sty" >> $OUTPUT
kpsewhich --all -engine luatex -progname lualatex gregoriotex.sty >> $OUTPUT 2>&1
echo "" >> $OUTPUT
echo "####	kpsewhich --all -engine luatex gregoriotex.tex" >> $OUTPUT
kpsewhich --all -engine luatex gregoriotex.tex >> $OUTPUT 2>&1
echo "" >> $OUTPUT
echo "" >> $OUTPUT
echo "-----------------------------------------------------------------------------" >> $OUTPUT
echo "" >> $OUTPUT
echo "" >> $OUTPUT

echo ""
echo ""
echo "system-setup.log created and saved the working directory from which you"
echo "ran this script.  Please email it to gregorio-users@gna.org as part of"
echo "your bug report."
echo ""
echo "You can also create an issue at "
echo "http://github.org/gregorio-project/gregorio/issues"
echo "and copy-paste the content of this file into the description."
echo "" 
