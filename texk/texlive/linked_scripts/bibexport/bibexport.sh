#!/bin/sh
##
## This is file `bibexport.sh',
## generated with the docstrip utility.
##
## The original source files were:
##
## bibexport.dtx  (with options: `script')
## 
## (c) 2005/02/27 Nicolas Markey <markey at lsv dot ens-cachan dot fr>
## All rights reserved.
## 
## This work may be distributed and/or modified under the conditions of
## the LaTeX Project Public License, either version 1.3 of this license
## or (at your option) any later version. The latest version of the
## license is in
## 
##     http://www.latex-project.org/lppl.txt
## 
## and version 1.3 or later is part of all distributions of LaTeX
## version 2003/12/01 or later.
## 
## \CharacterTable
##  {Upper-case    \A\B\C\D\E\F\G\H\I\J\K\L\M\N\O\P\Q\R\S\T\U\V\W\X\Y\Z
##   Lower-case    \a\b\c\d\e\f\g\h\i\j\k\l\m\n\o\p\q\r\s\t\u\v\w\x\y\z
##   Digits        \0\1\2\3\4\5\6\7\8\9
##   Exclamation   \!     Double quote  \"     Hash (number) \#
##   Dollar        \$     Percent       \%     Ampersand     \&
##   Acute accent  \'     Left paren    \(     Right paren   \)
##   Asterisk      \*     Plus          \+     Comma         \,
##   Minus         \-     Point         \.     Solidus       \/
##   Colon         \:     Semicolon     \;     Less than     \<
##   Equals        \=     Greater than  \>     Question mark \?
##   Commercial at \@     Left bracket  \[     Backslash     \\
##   Right bracket \]     Circumflex    \^     Underscore    \_
##   Grave accent  \`     Left brace    \{     Vertical bar  \|
##   Right brace   \}     Tilde         \~}
##
function usage()
{
echo "bibexport: a tool to extract BibTeX entries out of .bib files.
usage: $0 [-h|v] [-n] [-b bst] [-a [-e file]...] [-o file] file...
  -a, --all              export the entire .bib files
  -b, --bst              specifies the .bst style file [default: export.bst]
  -c, --crossref         include entries that are crossref'd  [default: yes]
  -e file, --extra file  extra .bib files to be used (for crossrefs)
  -n, --no-crossref      don't include crossref'd entries      [default: no]
  -o file                write output to file       [default: bibexport.bib]
  -p, --preamble         write a preamble at beginning of output
  -t, --terse            operate silently
  -h, --help             print this message and exit
  -v, --version          print version number and exit";
exit 0;
}
VERSION="2.20";
VDATE="2009/10/11";
CROSSREF="1";
FILE="";
EXT=".aux";
EXTRA="";
EXTRABIB="";
SPACE=" ";
BST="export";
TERSE="";
NOBANNER="true";
ARGS=$@;
if [ $# -eq 0 ]; then
  usage;
fi
while [ $# != 0 ]; do
        case $1 in
              -a|--all)
                EXT=""; SPACE="";
                shift ;;
            -b|--bst)
                if [ "`dirname $2`" = "." ]; then
                    DOLLARTWO="`basename $2 .bst`";
                else
                    DOLLARTWO="`dirname $2`/`basename $2 .bst`";
                fi
                BST="${DOLLARTWO}";
                shift 2;;
            -e|--extra)
                if [ "`dirname $2`" = "." ]; then
                    DOLLARTWO="`basename $2 .bib`";
                else
                    DOLLARTWO="`dirname $2`/`basename $2 .bib`";
                fi
                EXTRA="${EXTRA}${DOLLARTWO},";
                EXTRABIB="${EXTRABIB},${DOLLARTWO}.bib";
                shift 2;;
            -o|--output-file)
                if [ "`dirname $2`" = "." ]; then
                    DOLLARTWO="`basename $2 .bib`";
                else
                    DOLLARTWO="`dirname $2`/`basename $2 .bib`";
                fi
                OUTPUT="${DOLLARTWO}.bib";
                shift 2 ;;
            -c|--crossref|--crossrefs|--with-crossref|--with-crossrefs)
                CREF="1" ;
                shift ;;
            -n|--no-crossref|--without-crossref)
                CREF="20000" ;
                shift ;;
            -v|--version)
                echo "This is bibexport v${VERSION} (released ${VDATE})"; exit 0;;
            -p|--preamble)
                NOBANNER="false";
                shift ;;
            -t|--terse)
                TERSE=" -terse ";
                shift ;;
            -*)
                usage;;
            *)
                if [ "`dirname $1`" = "." ]; then
                    DOLLARONE="`basename $1 ${EXT}`";
                else
                    DOLLARONE="`dirname $1`/`basename $1 ${EXT}`";
                fi
                FILE="${FILE}${SPACE}${DOLLARONE}${EXT}";
                if [ -z "${SPACE}" ]; then
                    SPACE=",";
                fi;
                shift;;
        esac
done
FINALFILE=${OUTPUT};
if [ ! "${FINALFILE}" ]; then
    FINALFILE="bibexport.bib";
fi
TMPFILE="bibexp.`date +%s`";
if [ -z "${EXT}" ]; then
    if [ -z "${EXTRA}" ]; then
        cat > ${TMPFILE}.aux <<EOF
\citation{*}
\bibdata{${FILE}}
\bibstyle{${BST}}
EOF
    else
        cat > ${TMPFILE}.aux <<EOF
\citation{*}
\bibdata{${FILE}}
\bibstyle{expkeys}
EOF
        bibtex -min-crossrefs=${CREF} -terse ${TMPFILE};
        mv -f ${TMPFILE}.bbl ${TMPFILE}.aux;
        cat >> ${TMPFILE}.aux <<EOF
\bibdata{${EXTRA}${FILE}${EXTRABIB}}
\bibstyle{${BST}}
EOF
    fi
else
    cat ${FILE} | sed -e "s/bibstyle{.*}/bibstyle{${BST}}/" > ${TMPFILE}.aux;
fi
bibtex -min-crossrefs=${CREF} ${TERSE} ${TMPFILE}
echo "" > ${FINALFILE}
if [ ! "${NOBANNER}" = "true" ]; then
    echo -ne "@comment{generated using bibexport:\n" >> ${FINALFILE};
    echo -ne "  creation date:\t`date +\"%c\"`\n" >> ${FINALFILE};
    echo -ne "  command:\t\t$0 ${ARGS}\n" >> ${FINALFILE};
    echo -ne "  bibexport-version:\tv${VERSION} (${VDATE})\n" >> ${FINALFILE};
    echo -ne "  bibexport-maintainer:\tmarkey(at)lsv.ens-cachan.fr\n" >> ${FINALFILE};
    sed -ie "s/}/)/g" ${FINALFILE};
    echo -ne "}\n\n\n" >> ${FINALFILE};
fi
if [ ! "x${CREF}" = "x1" ]; then
    sed -r -e \
        "/^ *[cC][rR][oO][sS][sS][rR][eE][fF] *= *[^,]+,?$/d" \
        ${TMPFILE}.bbl >> ${FINALFILE};
else
    cat ${TMPFILE}.bbl >> ${FINALFILE};
fi
rm -f ${TMPFILE}.bbl ${TMPFILE}.aux ${TMPFILE}.blg
## 
##
## End of file `bibexport.sh'.
