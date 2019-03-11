#!/usr/bin/env bash

# Copyright (C) 2015-2019 The Gregorio Project (see CONTRIBUTORS.md)
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
# In special situations, you may want to skip some parts of the installation.
# To do this, set the SKIP environment variable to a comma-separated list of
# the parts you want don't want installed: tex, latex, fonts, docs, examples,
# and/or font-sources
#
# Setting the GENERATE_UNINSTALL environment variable to "false" will bypass
# uninstall script generation and any existing uninstall script will be left
# alone.

VERSION=`head -1 .gregorio-version`
FILEVERSION=`echo $VERSION | sed 's/\./_/g'`

TEXFILES=(tex/gregoriotex*.tex tex/gsp-default.tex tex/gregoriotex*.lua
          tex/*.dat)
LATEXFILES=(tex/gregorio*.sty)
TTFFILES=(fonts/*.ttf)
DOCFILES=(doc/*.tex doc/*.lua doc/*.gabc doc/*.pdf doc/doc_README.md)
EXAMPLEFILES=(examples/FactusEst.gabc examples/PopulusSion.gabc
              examples/main-lualatex.tex examples/debugging.tex)
FONTSRCFILES=(greextra.sfd squarize.py convertsfdtottf.py gregall.sfd
              gresgmodern.sfd fonts_README.md)
FONTSRCFILES=("${FONTSRCFILES[@]/#/fonts/}")
FONTSRCFILES+=(fonts/*-base.sfd)
# Files which have been eliminated, or whose installation location have been
# changed.  We will remove existing versions of these files in the target texmf
# tree before installing.
LEGACYFILES=(tex/luatex/gregoriotex/gregoriotex.sty
             tex/luatex/gregoriotex/gregoriosyms.sty
             tex/luatex/gregoriotex/gregoriotex-ictus.tex
             fonts/truetype/public/gregoriotex/parmesan.ttf
             fonts/truetype/public/gregoriotex/parmesan-op.ttf
             fonts/source/gregoriotex/parmesan-base.sfd
             fonts/truetype/public/gregoriotex/gresym.ttf
             fonts/truetype/public/gregoriotex/gregorio.ttf
             fonts/truetype/public/gregoriotex/gregorio-op.ttf
             fonts/source/gregoriotex/gregorio-base.sfd
             fonts/truetype/public/gregoriotex/granapadano.ttf
             fonts/truetype/public/gregoriotex/granapadano-op.ttf
             fonts/source/gregoriotex/granapadano-base.sfd)

NAME=${NAME:-gregoriotex}
FORMAT=${FORMAT:-luatex}
LATEXFORMAT=${LATEXFORMAT:-lualatex}
TEXHASH=${TEXHASH:-texhash}
KPSEWHICH=${KPSEWHICH:-kpsewhich}
CP=${CP:-cp}
RM=${RM:-rm}

GENERATE_UNINSTALL=${GENERATE_UNINSTALL:-true}
AUTO_UNINSTALL=${AUTO_UNINSTALL:-false}
REMOVE_OLD_FILES=${REMOVE_OLD_FILES:-true}

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
    echo
    echo "Please read the documentation in the script for additional options"
    exit 1
fi

UNINSTALL_SCRIPT_DIR="scripts/gregoriotex"
UNINSTALL_SCRIPT_FILE="uninstall-gtex.sh"
UNINSTALL_SCRIPT="${TEXMFROOT}/${UNINSTALL_SCRIPT_DIR}/${UNINSTALL_SCRIPT_FILE}"

function die {
    echo 'Failed.'
    exit 1
}

function install_to {
    dir="$1"
    shift
    mkdir -p "${TEXMFROOT}/$dir" || die
    $CP "$@" "${TEXMFROOT}/$dir" || die

    if ${GENERATE_UNINSTALL}
    then
        for name in "$@"
        do
            echo '$RM'" $dir/$(basename $name)" >> "${UNINSTALL_SCRIPT}"
        done
        echo "rmdir -p $dir 2> /dev/null || true" >> "${UNINSTALL_SCRIPT}"
        echo >> "${UNINSTALL_SCRIPT}"
    fi
}

function find_and_remove {
    for files in "$@"; do
        target="${TEXMFROOT}/${files}"
#        echo "Looking for $target"
        if [ -e "$target" ]; then
#            echo "Removing $target"
            $RM -f "$target"
        fi
    done
}

function not_installing {
    echo "install-gtex.sh: not installing $@"
}

if ${GENERATE_UNINSTALL}
then
    if [ -e "${UNINSTALL_SCRIPT}" ]
    then
        echo "${UNINSTALL_SCRIPT} exists."
        echo "This suggests that some version of GregorioTeX is already installed."
        if ${AUTO_UNINSTALL}
        then
            echo "AUTO_UNINSTALL=true, so uninstalling the old version of GregorioTeX."
            bash "${UNINSTALL_SCRIPT}"
        else
            echo "Re-run this script setting the environment variable AUTO_UNINSTALL=true"
            echo "to automatically uninstall the other version before installing the new one,"
            echo "or clean up the old installation files manually."
            exit 1
        fi
    fi

    mkdir -p "${TEXMFROOT}/${UNINSTALL_SCRIPT_DIR}" || die
    echo '# This script uninstalls GregorioTeX.' > "${UNINSTALL_SCRIPT}"
    echo '# Run it with "bash /path/to/uninstall-gtex.sh".' >> "${UNINSTALL_SCRIPT}"
    echo >> "${UNINSTALL_SCRIPT}"
    echo 'RM=${RM:-rm}' >> "${UNINSTALL_SCRIPT}"
    echo 'TEXHASH=${TEXHASH:-texhash}' >> "${UNINSTALL_SCRIPT}"
    echo >> "${UNINSTALL_SCRIPT}"
    echo 'cd $(dirname ${BASH_SOURCE[0]})/../..' >> "${UNINSTALL_SCRIPT}"
    echo >> "${UNINSTALL_SCRIPT}"
elif [ "$arg" != 'tds' ]
then
    echo "Not generating "${UNINSTALL_SCRIPT}""
fi

if ${REMOVE_OLD_FILES}
then
    echo "Removing old files."
    find_and_remove "${LEGACYFILES[@]}"
fi

declare -A skip_install
if [ -n "$SKIP" ]
then
    IFS=, read -r -a skip <<< "$SKIP"
    for skipped in "${skip[@]}"
    do
        # trim spaces
        skipped="${skipped#"${skipped%%[![:space:]]*}"}"
        skipped="${skipped%"${skipped##*[![:space:]]}"}"
        skip_install[$skipped]=true
    done
fi

echo "Installing in '${TEXMFROOT}'."
${skip_install[tex]:-false} && not_installing tex files ||
    install_to "tex/${FORMAT}/${NAME}" "${TEXFILES[@]}"
${skip_install[latex]:-false} && not_installing latex files ||
    install_to "tex/${LATEXFORMAT}/${NAME}" "${LATEXFILES[@]}"
${skip_install[fonts]:-false} && not_installing fonts ||
    install_to "fonts/truetype/public/${NAME}" "${TTFFILES[@]}"
${skip_install[docs]:-false} && not_installing docs ||
    install_to "doc/${FORMAT}/${NAME}" "${DOCFILES[@]}"
${skip_install[examples]:-false} && not_installing examples ||
    install_to "doc/${FORMAT}/${NAME}/examples" "${EXAMPLEFILES[@]}"
${skip_install[font-sources]:-false} && not_installing font sources ||
    install_to "fonts/source/${NAME}" "${FONTSRCFILES[@]}"

if ${GENERATE_UNINSTALL}
then
    echo '$RM'" ${UNINSTALL_SCRIPT_DIR}/${UNINSTALL_SCRIPT_FILE}" >> "${UNINSTALL_SCRIPT}"
    echo "rmdir -p ${UNINSTALL_SCRIPT_DIR} 2> /dev/null || true" >> "${UNINSTALL_SCRIPT}"
    echo >> "${UNINSTALL_SCRIPT}"

    echo '${TEXHASH}' >> "${UNINSTALL_SCRIPT}"
fi

if [ "$arg" = 'tds' ]
then
    echo "Making TDS-ready archive ${TDS_ZIP}."
    rm -f ${TDS_ZIP}
    (rm ${TEXMFROOT}/fonts/source/gregoriotex/gregorio-base.sfd ${TEXMFROOT}/fonts/source/gregoriotex/granapadano-base.sfd ) || die
    (cd ${TEXMFROOT} && zip -9 ../${TDS_ZIP} -q -r .) || die
    rm -r ${TEXMFROOT} || die
else
    ${TEXHASH} || die
fi
