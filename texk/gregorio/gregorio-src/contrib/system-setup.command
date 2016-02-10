#!/usr/bin/env bash

# This script generates a log detailing information about your computer.
# You can run it by double clicking on it on a Mac.
# On Linux this behavior is controlled by a preference.  See http://askubuntu.com/questions/286621/how-do-i-run-executable-scripts-in-nautilus for details.
# If prompted, you need to select "Run in Terminal" to see the output.

#This trap combination allows the window to linger long enough for the user to
#inspect the output, but still get closed when all is said and done.
function quit {
    read -n1 -r -p "Press any key to close window." key
    if $mac; then
        osascript -e 'tell application "Terminal" to close front window' > /dev/null 2>&1 &
    else
        exit
    fi
}

trap quit EXIT

case "$(uname -s)" in
    Darwin)
        echo 'Mac OS X detected'
        mac=true
        ;;
    Linux)
        echo 'Linux detected'
        mac=false
        ;;
    *)
        echo 'Unsupported OS detected'
        exit 1
        ;;
esac


HERE="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
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

files="gregorio-vowels.dat
gregoriosyms.sty
gregoriotex-chars.tex
gregoriotex-main.tex
gregoriotex-ictus.tex
gregoriotex-nabc.lua
gregoriotex-nabc.tex
gregoriotex-signs.lua
gregoriotex-signs.tex
gregoriotex-spaces.tex
gregoriotex-syllable.tex
gregoriotex-symbols.lua
gregoriotex-symbols.tex
gregoriotex.lua
gregoriotex.sty
gregoriotex.tex
gsp-default.tex
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
exit 0

