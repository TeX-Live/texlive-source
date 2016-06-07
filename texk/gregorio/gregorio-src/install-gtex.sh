#!/bin/bash

# Copyright (C) 2015 The Gregorio Project (see CONTRIBUTORS.md)
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

# This script installs the GregorioTeX portion of Gregorio.
#
# There are four ways to use this script:
#
# install-gtex.sh var:{tex-variable}
#
#   Installs GregorioTeX into the directory named by the given {tex-variable}.
#   If the DESTDIR environment variable is set, it will be prepended.
#
#   Example: install-gtex.sh var:TEXMFLOCAL
#   - Installs GregorioTeX into the system-wide TEXMF directory
#
#   Example: install-gtex.sh var:TEXMFHOME
#   - Installs GregorioTeX into the user's personal TEXMF directory
#
# install-gtex.sh system|user
#
#   Installs GregorioTeX into one of two common install locations.  If the
#   DESTDIR environment variable is set, it will be prepended.
#
#   Example: install-gtex.sh system
#   - Installs GregorioTeX into the system-wide TEXMF directory; an alias for
#     install-gtex.sh var:TEXMFLOCAL
#
#   Example: install-gtex.sh user
#   - Installs GregorioTeX into the user's personal TEXMF directory; an alias
#     for install-gtex.sh var:TEXMFHOME
#
# install-gtex.sh dir:{directory}
#
#   Installs GregorioTeX into the {directory} directory.
#
#   Example: install-gtex.sh dir:/tmp/gtex
#   - Installs GregorioTeX into /tmp/gtex
#
# install-gtex.sh tds
#
#   Creates a TDS-ready archive named gregoriotex.tds.zip
#

VERSION=`head -1 .gregorio-version`
FILEVERSION=`echo $VERSION | sed 's/\./_/g'`

TEXFILES=(tex/gregoriotex*.tex tex/gsp-default.tex tex/gregoriotex*.lua
          tex/*.dat)
LATEXFILES=(tex/gregorio*.sty)
TTFFILES=(gregorio.ttf greciliae.ttf granapadano.ttf gregorio-op.ttf
          greciliae-op.ttf granapadano-op.ttf greextra.ttf gregall.ttf
          gresgmodern.ttf)
DOCFILES=(doc/Appendix*.tex doc/Command*.tex doc/Gabc.tex
          doc/*Ref.tex doc/*Ref.lua doc/*.gabc
          doc/Gregorio*Ref.pdf)
EXAMPLEFILES=(examples/FactusEst.gabc examples/PopulusSion.gabc
              examples/main-lualatex.tex examples/debugging.tex)
FONTSRCFILES=(gregorio-base.sfd granapadano-base.sfd greciliae-base.sfd
              greextra.sfd squarize.py convertsfdtottf.py gregall.sfd
              gresgmodern.sfd README.md)
# Files which have been eliminated, or whose installation location have been
# changed.  We will remove existing versions of these files in the target texmf
# tree before installing.
LEGACYFILES=(tex/luatex/gregoriotex/gregoriotex.sty
             tex/luatex/gregoriotex/gregoriosyms.sty
             tex/luatex/gregoriotex/gregoriotex-ictus.tex
             fonts/truetype/public/gregoriotex/parmesan.ttf
             fonts/truetype/public/gregoriotex/parmesan-op.ttf
             fonts/source/gregoriotex/parmesan-base.sfd)

NAME=${NAME:-gregoriotex}
FORMAT=${FORMAT:-luatex}
LATEXFORMAT=${LATEXFORMAT:-lualatex}
TEXHASH=${TEXHASH:-texhash}
KPSEWHICH=${KPSEWHICH:-kpsewhich}
CP=${CP:-cp}
RM=${RM:-rm}

TTFFILES=("${TTFFILES[@]/#/fonts/}")
FONTSRCFILES=("${FONTSRCFILES[@]/#/fonts/}")

arg="$1"
case "$arg" in
    system)
        arg='var:TEXMFLOCAL'
        ;;
    user)
        arg='var:TEXMFHOME'
        ;;
esac

case "$arg" in
    "")
        ;;
    tds)
        TDS_ZIP="${NAME}.tds.zip"
        TEXMFROOT=./tmp-texmf
        ;;
    var:*)
        TEXMFROOT=`${KPSEWHICH} -expand-path "\$${arg#var:}"`
        if [ "$TEXMFROOT" = "" ]
        then
            TEXMFROOT=`${KPSEWHICH} -var-value "${arg#var:}"`
        fi
        if [ "$TEXMFROOT" = "" ]
        then
            echo "Invalid TeX variable: '${arg#var:}'"
            echo
        else
            sep=`${KPSEWHICH} -expand-path "{.,.}"`
            sep="${sep#.}"
            sep="${sep%.}"
            TEXMFROOT="${DESTDIR}${TEXMFROOT/${sep}*/}"
        fi
        ;;
    dir:*)
        TEXMFROOT="${arg#dir:}"
        ;;
    *)
        echo "Invalid argument: '$arg'"
        echo
        ;;
esac

if [ "$TEXMFROOT" = "" ]
then
    echo "Usage: $0 var:{tex-variable}"
    echo "       $0 dir:{directory}"
    echo "       $0 system|user|tds"
    exit 1
fi

function die {
    echo 'Failed.'
    exit 1
}

function install_to {
    dir="$1"
    shift
    mkdir -p "$dir" || die
    $CP "$@" "$dir" || die
}

function find_and_remove {
    for files in $1; do
        target="${TEXMFROOT}/${files}"
        if [ -e "$target" ]; then
            $RM -f "$target"
        fi
    done
}

echo "Removing old files"
find_and_remove "${LEGACYFILES[@]}"

echo "Installing in '${TEXMFROOT}'."
install_to "${TEXMFROOT}/tex/${FORMAT}/${NAME}" "${TEXFILES[@]}"
install_to "${TEXMFROOT}/tex/${LATEXFORMAT}/${NAME}" "${LATEXFILES[@]}"
install_to "${TEXMFROOT}/fonts/truetype/public/${NAME}" "${TTFFILES[@]}"
install_to "${TEXMFROOT}/doc/${FORMAT}/${NAME}" "${DOCFILES[@]}"
install_to "${TEXMFROOT}/doc/${FORMAT}/${NAME}/examples" "${EXAMPLEFILES[@]}"
install_to "${TEXMFROOT}/fonts/source/${NAME}" "${FONTSRCFILES[@]}"

if [ "$arg" = 'tds' ]
then
    echo "Making TDS-ready archive ${TDS_ZIP}."
    rm -f ${TDS_ZIP}
    (cd ${TEXMFROOT} && zip -9 ../${TDS_ZIP} -q -r .) || die
    rm -r ${TEXMFROOT} || die
else
    ${TEXHASH} || die
fi
