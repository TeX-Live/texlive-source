#!/bin/sh
# 
#                            checklistings                               #
#                             version 1.0                                #
#                                                                        #
#                     Timothy Bourke <tim@tbrk.org>                      #
#                                Inria                                   #
#                       École normale supérieure                         #
#                                                                        #
#                      Marc Pouzet <pouzet@ens.fr>                       #
#                   Université Pierre et Marie Curie                     #
#                       École normale supérieure                         #
#                                Inria                                   #
#

##
# Configuration
SCRIPT=$(basename "$0")
SUFFIX=.chkl

RED="[0;31m"
BLUE="[0;34m"
BLACK="[0;0m"

ADJUSTLINETEXT=line

##
# Command-line

usage () {
    printf '%s: check source code exported from LaTeX documents.\n' "$SCRIPT"
    printf 'usage: %s [options] [file ...]\n' "$SCRIPT"
    printf '\n'
    printf '  options:\n'
    printf '  --no-colors       log to terminal without control codes\n'
    printf '  --no-line-adjust	do not try to adjust error line numbers\n'
    printf '\n'
}

unset INFILES
while [ -n "$1" ]; do
  case "$1" in
  --no-colors)
    unset RED BLUE BLACK
    ;;
  --no-line-adjust)
    unset ADJUSTLINETEXT
    ;;
  --h)
    usage
    exit 0
    ;;
  --help)
    usage
    exit 0
    ;;
  -*)
    printf "%s: unrecognized option '%s'\n" "$SCRIPT" "$1"
    usage
    exit 1
    ;;
  *${SUFFIX})
    INFILES="$INFILES $1"
    ;;
  *)
    INFILES="$INFILES $1${SUFFIX}"
    ;;
  esac
  shift
done

if [ -z "$INFILES" ]; then
    INFILES=$(ls ./*${SUFFIX})
fi

##
# Allocate temporary files

cleanup() {
    rm -f "${OUTFILE}" "${ERRFILE}"
}
OUTFILE=$(mktemp -t "${SCRIPT}.out.XXXXXX") || exit 1
ERRFILE=$(mktemp -t "${SCRIPT}.err.XXXXXX") || (cleanup; exit 1)
trap cleanup 1 2 15

##
# Code

#
# Given
#   var	    a variable to store the result
#   name    the filename prefix
#   num	    an id number
# generate a filename from name and num and store it in var.
#
setfilename() {
    var="$1"
    name="$2"
    num="$3"
    if [ "$num" -lt 10 ]; then
	eval "$var='${name}000${num}'"
    elif [ "$num" -lt 100 ]; then
	eval "$var='${name}00${num}'"
    elif [ "$num" -lt 1000 ]; then
	eval "$var='${name}0${num}'"
    else
	eval "$var='${name}${num}'"
    fi
}

#
# If the WITHOPEN option is set, create a modified program text (with a
# different name) whose first lines import dependencies.
# Args:
#   filename	    the id of the program text
#   openfilenums    its dependencies (id list)
#
addopen() {
    filename="$1"
    openfilenums="$2"
    unset of f

    if [ -n "$WITHOPEN" ]; then
	setfilename f "$PREFIX" "$filename"
	if [ -f "$SUBDIR$f$EXT" ]; then

	    openfiles=""
	    for n in $openfilenums; do
		setfilename of "$WITHOPEN" "$n"
		# shellcheck disable=SC2154
		openfiles="$openfiles ${INCLUDECMD} $of"
	    done

	    setfilename of "$WITHOPEN" "$filename"
	    if [ -n "$openfiles" ]; then
		addedlines=$((addedlines + 1))
		printf '%s \n' "$openfiles" > "$SUBDIR$of$EXT"
		cat "$SUBDIR$f$EXT" >> "$SUBDIR$of$EXT"
	    else
		cat "$SUBDIR$f$EXT" > "$SUBDIR$of$EXT"
	    fi
	fi
    fi
}

# A filename string for sed that ignores the case (BSD sed has no i flag)
makeicfilename() {
    IFILENAME=""
    for l in $(printf "%s" "$FILENAME" | sed -e 's/\(.\)/\1 /g'); do
	L="$(printf "%s" "$l" | tr "[:lower:]" "[:upper:]")"
	IFILENAME="${IFILENAME}[$L$l]"
    done
}

#
# Given
#   $ERRFILE	     path to the file containing error messages
#   $ADJUSTLINETEXT  the text preceding the line number (default: "line")
#		     if empty or unset, no adjustments are made.
#   $addedlines      the amount by which to decrement line numbers
#
adjustlinenumbers() {
    if [ -n "$ADJUSTLINETEXT" ]; then
	printf '' > "$ERRFILE-adjusted" 
	while read line
	do
	    linen=$(expr "$line" : ".*$ADJUSTLINETEXT  *\([0-9][0-9]*\).*")
	    if [ -n "$linen" ]; then
		before=$(expr "$line" : "^\(.*$ADJUSTLINETEXT  *\)$linen.*$")
		after=$(expr "$line" : "^.*$ADJUSTLINETEXT  *$linen\(.*\)$")
		newlinen=$((linen - addedlines))
		printf "%s%s%s\n" "$before" "$newlinen" "$after" \
		    >> "$ERRFILE-adjusted" 
	    else
		printf "%s\n" "$line" >> "$ERRFILE-adjusted" 
	    fi
	done < "$ERRFILE"
	mv "$ERRFILE-adjusted" "$ERRFILE"
    fi
}

#
# Given
#   num		    a program (id)
#   openfilenums    its dependencies (id list)
# (Try to) Compile the program and create a corresponding .tex file that
# summarizes the results for reading back into the source document.
#
compile() {
    num="$1"
    openfilenums="$2"
    unset ifile ofile outf of FILENAME

    # Decide whether to use the original program source or the version
    # augmented to import dependencies.
    if [ -n "$WITHOPEN" ]; then
	FILENAME=${WITHOPEN}
    else
	FILENAME=${PREFIX}
    fi
    makeicfilename

    # Generate the program (ifile) and output (ofile) filenames
    setfilename ifile "$FILENAME" "$num"
    # shellcheck disable=SC2154
    ipath="$SUBDIR$ifile$EXT" 
    setfilename ofile "$PREFIX" "$num"
    # shellcheck disable=SC2154
    opath="$SUBDIR$ofile.tex"
    opath_msg="$SUBDIR$ofile.msg"
    opath_err="$SUBDIR$ofile.err"
    opath_html="$SUBDIR$ofile.html"

    # Check that the input file exists
    if [ -f "$ipath" ]; then
	printf '> %s%s...%s\n' "$BLUE" "$ofile$EXT" "$BLACK"
    else
	printf '%s> %s%s: program source not found.%s\n' \
	    "$RED" "$ofile" "$EXT" "$BLACK"
	return 1
    fi

    # Generate a list of dependency filenames to pass to the compiler
    openfiles=""
    for n in $openfilenums; do
	setfilename of "$FILENAME" "$n"
	openfiles="$openfiles $SUBDIR$of$EXT"
    done

    # Invoke the compiler
    # shellcheck disable=SC2086
    $COMPILER $COMPILERFLAGS $openfiles $LASTFLAGS "$ipath" \
	>"$OUTFILE" 2>"$ERRFILE"
    COMPILERSTATUS=$?
    adjustlinenumbers

    # Start the output tex file with the compilation command as a comment
    printf '%% %s\n' "$COMPILER $COMPILERFLAGS $openfiles \
	$LASTFLAGS $ipath ($COMPILERSTATUS)" > "$opath"

    # Signal compilation success (\chklistingtrue) or not (\chklistingfalse)
    if [ "$COMPILERSTATUS" -eq 0 ]; then
	printf '\\chklistingtrue\n'   >> "$opath"
	if [ "$SHOULDFAIL" -eq 1 ]; then
	    printf '%s  unexpected success (line %s/ page %s)!%s\n' \
		"$RED" "$LINENUM" "$PAGENUM" "$BLACK" >&2
	fi
    else
	printf '\\chklistingfalse\n'  >> "$opath"
	if [ "$SHOULDFAIL" -eq 0 ]; then
	    printf "%s  unexpected failure (line %s / page %s)!%s\n" \
		"$RED" "$LINENUM" "$PAGENUM" "$BLACK" >&2
	    while read line
	    do
	      printf "  | %s\n" "$line"
	    done < "$ERRFILE" >&2
	fi
    fi

    # Include a sanitized compilation command (\setchklistingcmd)
    printf '\setchklistingcmd{%s %s \\chklistingfile}\n' \
	"${COMPILERNAME}" "${LASTFLAGS}" >> "$opath"

    # Include the compiler's stdout
    printf "%s\n" '\begin{ChkListingMsg}'	      >> "$opath"
    if [ "$(wc -l < "$OUTFILE")" -eq 0 ]; then
	printf "Failed.\n"			      >> "$opath"
    else
	sed -e "s#$SUBDIR$ifile#$PREFIX#g"		\
	    -e "s#$IFILENAME[0-9][0-9][0-9][0-9]\.##g"	\
	    "$OUTFILE"				      > "$opath_msg"
	cat "$opath_msg"			      >> "$opath"
    fi
    printf "%s\n" '\end{ChkListingMsg}'		      >> "$opath"

    # Include the compiler's stderr
    printf "%s\n" '\begin{ChkListingErr}'	      >> "$opath"
    if [ "$(wc -l < "$ERRFILE")" -eq 0 ]; then
	printf "%s\n" "Success."		      >> "$opath"
    else
	sed -e "s#$SUBDIR$ifile#$PREFIX#g"		\
	    -e "s#$IFILENAME[0-9][0-9][0-9][0-9]\.##g"	\
	    "$ERRFILE"				      > "$opath_err"
	cat "$opath_err"			      >> "$opath"
    fi
    printf "%s\n" '\end{ChkListingErr}'		      >> "$opath"

    # Filter to html if necessary
    if [ -n "$HTMLFILTER" ]; then
	# shellcheck disable=SC2086
	if [ "$HTMLFILTER" = "cat" ]; then
	    printf '<pre>\n'	    > "$opath_html"
	    $HTMLFILTER "$ipath"    >> "$opath_html"
	    printf '</pre>\n'	    >> "$opath_html"
	else
	    $HTMLFILTER "$ipath" > "$opath_html"
	fi
    fi

    return 0
}

locked=
dooption() {
    opt_name="$1"
    opt_value="$2"

    case "$opt_name" in
	lock\ *)
	    opt_name=${opt_name#lock }
	    locked="$locked $opt_name "
	    ;;
	*)
	    if [ "$(expr "$locked" : ".* $opt_name .*")" -ne 0 ]; then
		printf \
		    'info: %s: ignoring locked option ''%s''\n' \
		    "$infile" "$opt_name" >&2
		return
	    fi
	    ;;
    esac

    case "$opt_name" in
	subdir)
	    SUBDIR="$opt_value" ;;
	prefix)
	    PREFIX="$opt_value" ;;
	ext)
	    EXT="$opt_value" ;;
	compiler)
	    COMPILER="${opt_value}"
	    COMPILERNAME="$(basename "${COMPILER}")"
	    ;;
	compilerflags)
	    COMPILERFLAGS="${opt_value}" ;;
	lastflags)
	    LASTFLAGS="${opt_value}" ;;
	includecmd)
	    INCLUDECMD="${opt_value}"
	    if [ -z "$INCLUDECMD" ]; then
		WITHOPEN=
	    else
		WITHOPEN=Withopen
	    fi
	    ;;
	htmlfilter)
	    HTMLFILTER="${opt_value}" ;;
    esac
}

subdirs=
readchkl() {
    infile=$1
    unset l existing openfilenums filenum opennums

    # Process each line of the command file
    while read l; do
	addedlines=0
	case $l in
	    [[:digit:]]*:*)
		# a snippet to be compiled
		filenum=$(expr "$l" : '\(.*\):.*')
		openfilenums=$(expr "$l" : '.*:\(.*\)')

		# build up a list of dependencies in opennums
		opennums=""
		for n in $openfilenums; do
		    case $n in
		    \[page=*)
			PAGENUM=$(expr "$n" : '\[page=\(.*\)\]')
			if [ "$PAGENUM" = "" ]; then
			    PAGENUM="unknown"
			fi
			;;
		    \])
			# ignore invalid patterns when checklistings.sty
			# cannot expand \thepage properly (due to a
			# conflicting package setting) and instead produces
			# '[page=\thepage ]' which is read as two tokens:
			# '[page=\thepage' and ']'.
			;;
		    \[line=*\])
			LINENUM=$(expr "$n" : '\[line=\(.*\)\]')
			;;
		    \[fail\])
			SHOULDFAIL=1
			;;
		    \[skip=*\])
			addedlines=$(expr "$n" : '\[skip=\([0-9][0-9]*\)\]')
			;;
		    *)
			if [ "$n" -eq "$n" ] 2>/dev/null; then
			    opennums="$opennums $n"
			else
			    printf \
				"%swarning: %s: ignoring unresolved include '%s'%s\n" \
				"$RED" "$filenum" "$n" "$BLACK" >&2
			fi
			;;
		    esac
		done

		addopen "$filenum" "$opennums"
		compile "$filenum" "$opennums"
		;;

	    *=*)
		n="$(expr "$l" : '\([^=]*\)=.*')"
		v="$(expr "$l" :        '.*=\(.*\)')"
		dooption "$n" "$v"
		if [ "$n" = "subdir" ]; then
		    existing=$(expr "$subdirs" : ".*::${SUBDIR}:\\([^:]*\\):.*")
		    if [ -n "$existing" ]; then
			printf "%swarning: %s: subdir=%s already used by %s!%s\n" \
			    "$RED" "$infile" "$SUBDIR" "$existing" "$BLACK" >&2
		    fi
		fi
		;;

	    *)
		if [ -n "$l" ]; then
		    printf "%sbad %s: %s%s\n" "$RED" "$infile" "$l" "$BLACK" >&2
		fi
		;;
	esac
	unset PAGENUM
	unset LINENUM
	SHOULDFAIL=0
    done < "$1"
    subdirs="$subdirs::$SUBDIR:$infile:"
}

# Loop through each checklistings command file
for f in ${INFILES}; do

    # Add the suffix if necessary (as latex does)
    case $f in
	*${SUFFIX}) ;;
	*) infile="$f$SUFFIX" ;;
    esac

    readchkl "$f"
done

cleanup

