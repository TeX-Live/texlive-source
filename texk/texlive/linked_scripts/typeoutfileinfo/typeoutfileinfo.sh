%!/usr/bin/env bash
##
## This is `typeoutfileinfo.sh', a minimalistic shell script for Unices.
## 
##     ./typeoutfileinfo.sh [FILENAME].[EXT]
##
## runs latex with the readprov package
##
##     http://ctan.org/pkg/readprov
##
## in order to display [FILENAME].[TXT]'s file info (\listfile entry)
## using \typeout. This requires that [FILENAME].[TXT] contains a 
## \ProvidesFile, \ProvidesPackage, or \ProvidesClass command.
(
cat << EOM
\\RequirePackage{readprov} \\ReadFileInfos{$1}
\\typeout{^^J^^J *$1 info*: \\space \\csname ver@$1\\endcsname^^J^^J}\\stop
EOM
) | latex
##
## Copyright (C) 2012 Uwe Lueck, http://contact-ednotes.sty.de.vu/
##
## This program may be distributed and/or modified under the
## conditions of the LaTeX Project Public License, either version 1.2
## of this license or (at your option) any later version.
## The latest version of this license is in
##   http://www.latex-project.org/lppl.txt
## and version 1.2 or later is part of all distributions of LaTeX 
## version 1999/12/01 or later.
##
## There is NO WARRANTY.
##
## v0.1 as of 2012-03-13
