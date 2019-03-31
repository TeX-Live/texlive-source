#!/bin/bash
##
## This is file `bibexport.sh',
## generated with the docstrip utility.
##
## The original source files were:
##
## bibexport.dtx  (with options: `script')
## 
## (c) 2019/03/30 Nicolas Markey <bibexport at markey dot fr>
## 
## This work may  be distributed and/or modified under  the conditions of
## the LaTeX Project  Public License, either version 1.3  of this license
## or (at  your option)  any later version.   The latest version  of this
## license is in
## 
##   http://www.latex-project.org/lppl.txt
## 
## and version 1.3 or later is part of all distributions of LaTeX version
## 2005/12/01 or later.
## 
## This work has the LPPL maintenance status `maintained'.
## The Current Maintainer of this work is Nicolas Markey.
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
function checkversion()
{
  kpsewhich expcites.bst > /dev/null ||
    echo "-----------
--Warning-- file expcites.bst not found.
-----------"
  grep -q $VDATE `kpsewhich expkeys.bst` ||
    echo "-----------
--Warning-- the version of the .bst files does not match with that of this script.
-----------"
}
function usage()
{
echo "bibexport: a tool to extract BibTeX entries out of .bib files.
usage: `basename $0` [-h|v|n|c|a|d|s|t] [-b|e|es|ec|o|r file] file...

Basic options:
--------------
 -a, --all                  export the entire .bib files
 -o bib, --output-file bib  write output to file       [default: bibexport.bib]
 -ns, --nosave              overwrite output file without keeping a copy
 -p, --preamble             write a preamble at beginning of output
 -t, --terse                operate silently
 -h, --help                 print this message and exit
 -v, --version              print version number and exit

Advanced options:
-----------------
 -b bst, --bst bst          specifies the .bst style file [default: export.bst]
 -c, --crossref             preserve crossref field               [default: no]
 -n, --no-crossref          remove crossref'd entries             [default: no]
 -e bib, --extra bib        extra .bib file to be used (crossrefs and strings)
 -es bib, --extras bib      extra .bib file to be used (for strings)
 -ec bib, --extrac bib      extra .bib file to be used (for crossrefs)
 -r bib, --replace bib      replace .bib file(s) in the .aux file
 -d, --debug                create intermediate files but don't run BibTeX";
exit 0;
}
function opttoolate()
{
if [ ! -z "${TOOLATE}" ]; then
    echo "No options are allowed after the input files";
    exit 0;
fi
}
## Version number
VERSION="3.03";
## Release date
VDATE="2019/03/30";

# ALL is a flag set to 1 when '-a' is given
ALL="";
# FILE will be the main input file(s) (.aux or .bib, depending on '-a')
FILE="";
# EXT is the extension of the input file(s) (.aux, or .bib if '-a')
EXT=".aux";
# EXTRA and EXTRABIB are two copies of the extra files ('-e'), used to
# include crossref'd entries and @string's
EXTRA="";
EXTRABIB="";
# REPLACEBIB ('-r') is set to 1 when the \bibdata of the .aux input file
# must be ignores (then '-e' must be used)
REPLACEBIB="";
# NEWBIB will contain the argument given to -r
NEWBIB="";
# BST is the .bst file to be used (default to export.bst)
BST="export";
# TERSE will be set to '-terse' if '-t' is given
TERSE="";
# NOSAVE if no need to save file before overwriting it
NOSAVE=""
# BANNER is used to turn on or off the preamble informations in the output
BANNER="";
# CREF is the number of citations of crossrefs from which the crossref'd entry
# must be included.
CREF="0";

# SPACE will be either ' ' or ','
SPACE="";
# TOOLATE is used to prevent extra options after the main file
TOOLATE="";
# DEBUG is used to create files but not run BibTeX.
DEBUG="";

ARGS=$@;
if [ $# -eq 0 ]; then
  usage;
fi
checkversion;
while [ $# != 0 ]; do
    case $1 in
        -a|--all)
            ## - export all entries in the input file(s)
            ## - the input files are BibTeX files
            opttoolate;
            EXT=""; SPACE=""; ALL="a";
            shift ;;
        -b|--bst)
            ## - specifies the .bst file to use (default to 'export.bst')
            opttoolate;
            if [ "`dirname $2`" = "." ]; then
                DOLLARTWO="`basename $2 .bst`";
            else
                DOLLARTWO="`dirname $2`/`basename $2 .bst`";
            fi
            BST="${DOLLARTWO}";
            shift 2;;
        -d|--debug)
            ## - debug mode: we create files but do not run bibtex
            ## - instead, we print what we would have done...
            opttoolate;
            DEBUG="a";
            shift ;;
        -e|--extra)
            ## - extra input files (containing crossrefs or strings)
            ## - they will be included twice: once before the main file(s)
            ##   (for @string's), once after (for crossrefs). We fool BibTeX
            ##   by naming the first one 'file.bib' and the second one
            ##   'file.bib.bib', to avoid complaints.
            opttoolate;
            if [ "`dirname $2`" = "." ]; then
                DOLLARTWO="`basename $2 .bib`";
            else
                DOLLARTWO="`dirname $2`/`basename $2 .bib`";
            fi
            EXTRA="${EXTRA}${DOLLARTWO},";
            EXTRABIB="${EXTRABIB},${DOLLARTWO}.bib";
            shift 2;;
        -es|--extras)
            ## - extra input files (containing strings)
            ## - will be included *before* the main files (hence not suitable
            ##   for crossrefs)
            opttoolate;
            if [ "`dirname $2`" = "." ]; then
                DOLLARTWO="`basename $2 .bib`";
            else
                DOLLARTWO="`dirname $2`/`basename $2 .bib`";
            fi
            EXTRA="${EXTRA}${DOLLARTWO},";
            shift 2;;
        -ec|--extrac)
            ## - extra input files (containing crossrefs)
            ## - will be included only *after* the main files (hence not
            ##   suitable for @string's)
            opttoolate;
            if [ "`dirname $2`" = "." ]; then
                DOLLARTWO="`basename $2 .bib`";
            else
                DOLLARTWO="`dirname $2`/`basename $2 .bib`";
            fi
            EXTRABIB="${EXTRABIB},${DOLLARTWO}.bib";
            shift 2;;
        -o|--output-file)
            ## - name of the output file
            ## - we force it to end with '.bib'
            opttoolate;
            if [ "`dirname $2`" = "." ]; then
                DOLLARTWO="`basename $2 .bib`";
            else
                DOLLARTWO="`dirname $2`/`basename $2 .bib`";
            fi
            OUTPUT="${DOLLARTWO}.bib";
            shift 2 ;;
        -c|--crossref|--crossrefs|--with-crossref|--with-crossrefs)
            ## - whether or not to preserve 'crossref' keys.
            ## - by default, they are removed, but crossref'd entries are
            ##   included.
            ## - crossrefs are *always* expanded anyway.
            opttoolate;
            CREF="1" ;
            shift ;;
        -n|--no-crossref|--without-crossref|--no-crossrefs|--without-crossrefs)
            ## - to remove crossref'd entries (hence remove 'crossref' keys).
            opttoolate;
            CREF="20000" ;
            shift ;;
        -r|--replace)
            ## - to replace the file(s) given in \bibdata in the .aux file with
            ##   (a) new one(s).
            opttoolate;
            REPLACEBIB="a";
            if [ "`dirname $2`" = "." ]; then
                DOLLARTWO="`basename $2 .bib`";
            else
                DOLLARTWO="`dirname $2`/`basename $2 .bib`";
            fi
            NEWBIB="${NEWBIB}${DOLLARTWO}.bib,";
            shift 2;;
        -v|--version)
            echo "This is bibexport v${VERSION} (released ${VDATE})"; exit 0;;
        -ns|--nosave|--no-save)
            NOSAVE="a";
            shift ;;
        -p|--preamble|--with-preamble)
            BANNER="a";
            shift ;;
        -t|--terse|--silent)
            TERSE=" -terse ";
            shift ;;
        -*)
            usage;;
        *)
            ## - list of input files
            ## - we ensure that no extra option is given later...
            TOOLATE="a";
            if [ "`dirname $1`" = "." ]; then
                DOLLARONE="`basename $1 ${EXT}`";
            else
                DOLLARONE="`dirname $1`/`basename $1 ${EXT}`";
            fi
            FILE="${FILE}${SPACE}${DOLLARONE}${EXT}";
            if [ -z "${ALL}" ]; then
                SPACE=" ";
            else
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
if [ -z "${EXT}" ]; then ## we export all entries
    if [ -z "${EXTRA}" ]; then ## we have no extra files
        cat > ${TMPFILE}.aux <<EOF
\citation{*}
\bibdata{${FILE}}
\bibstyle{${BST}}
EOF
    else ## we have extra files (e.g. for crossrefs) but want all entries from ${FILE}
         ## we first extract the keys to be used:
        cat > ${TMPFILE}.aux <<EOF
\citation{*}
\bibdata{${FILE}}
\bibstyle{expkeys}
EOF
        ## This run may generate errors. We redirect the output:
        bibtex -min-crossrefs=${CREF} -terse ${TMPFILE} >/dev/null 2>&1;
        mv -f ${TMPFILE}.bbl ${TMPFILE}.aux;
        ## and then prepare the .aux file for exporting:
        cat >> ${TMPFILE}.aux <<EOF
\bibdata{${EXTRA}${FILE}${EXTRABIB}}
\bibstyle{${BST}}
EOF
    fi
else ## we only export entries listed in the given .aux file:
  if [ -z "${REPLACEBIB}" ]; then
    cat ${FILE} | sed -e "s/bibstyle{.*}/bibstyle{${BST}}/" > ${TMPFILE}.aux;
  else
    cat ${FILE} | sed -e "s/bibstyle{.*}/bibstyle{${BST}}/" \
      -e "s|bibdata{.*}|bibdata{${EXTRA}${NEWBIB%,}${EXTRABIB}}|" > ${TMPFILE}.aux;
  fi
fi
if [ -z "$DEBUG" ]; then
    bibtex -min-crossrefs=${CREF} ${TERSE} ${TMPFILE};
    if [ -e ${FINALFILE} ] && [ -z "${NOSAVE}" ]; then
        mv ${FINALFILE} ${FINALFILE}-save-`date "+%Y.%m.%d:%H.%M.%S"`
    fi
    echo "" > ${FINALFILE}
else
    echo "bibtex -min-crossrefs=${CREF} ${TERSE} ${TMPFILE};"
    if [ -e ${FINALFILE} ] && [ -z "${NOSAVE}" ]; then
        echo "mv ${FINALFILE} ${FINALFILE}-save-`date \"+%Y.%m.%d:%H.%M.%S\"`"
    fi
    echo "echo \"\" > ${FINALFILE}"
fi
if [ ! -z "${BANNER}" ]; then
    ## list of cited entries
    if [ -z "$DEBUG" ]; then
        sed -i -e "s/\\\bibstyle{.*}/\\\bibstyle{expcites}/" ${TMPFILE}.aux
        mv ${TMPFILE}.aux ${TMPFILE}-cites.aux
        bibtex -terse -min-crossrefs=${CREF} ${TMPFILE}-cites
        echo -ne "@comment{generated using bibexport:\n" >> ${FINALFILE};
        echo -ne "  creation date:\t`date +\"%c\"`\n" >> ${FINALFILE};
        echo -ne "  command:\t\t`basename $0` ${ARGS}\n" >> ${FINALFILE};
        if [ -z "${EXT}" ]; then
            echo -ne "  source files:\t\t${FILETAB}\t\t\t${EXTRABIBTAB}\n" >> ${FINALFILE}; \
                fi
        cat ${TMPFILE}-cites.bbl >> ${FINALFILE};
        #echo -ne "  bibexport-version:\tv${VERSION} (${VDATE})\n" >> ${FINALFILE};
        #echo -ne "  bibexport-maintainer:\tNicolas Markey <bibexport(at)markey.fr>\n" >> ${FINALFILE};
        sed -i -e "s/}/)/g" ${FINALFILE};
        echo -n -e "}\n\n\n" >> ${FINALFILE};
        rm -f ${TMPFILE}-cites.bbl ${TMPFILE}-cites.aux ${TMPFILE}-cites.blg
    fi
fi
if [ ${CREF} -ne 1 ]; then
    if [ -z "$DEBUG" ]; then
        egrep -iv '^ *crossref *= *[^,]+,?$' \
            ${TMPFILE}.bbl >> ${FINALFILE};
    else
        echo "egrep -iv '^ *crossref *= *[^,]+,?$' ${TMPFILE}.bbl >> ${FINALFILE};"
    fi
else
    if [ -z "$DEBUG" ]; then
        cat ${TMPFILE}.bbl >> ${FINALFILE};
    else
        echo "cat ${TMPFILE}.bbl >> ${FINALFILE};"
    fi
fi
if [ -z "$DEBUG" ]; then
    rm -f ${TMPFILE}.bbl ${TMPFILE}.aux ${TMPFILE}.blg;
else
    echo "rm -f ${TMPFILE}.bbl ${TMPFILE}.aux ${TMPFILE}.blg";
fi
## 
##
## End of file `bibexport.sh'.
