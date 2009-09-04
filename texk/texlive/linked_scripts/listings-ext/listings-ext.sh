#! /bin/sh
### listings-ext.sh ---

## Author: Dr. Jobst Hoffmann <j.hoffmann@fh-aachen.de>
## Version: $Id: listings-ext.dtx 48 2009-08-31 18:30:48Z ax006ho $
## Keywords: LaTeX, listings
## Copyright: (C) 2008-2009 Jobst Hoffmann, <j.hoffmann@fh-aachen.de>
##-------------------------------------------------------------------
##
## This file may be distributed and/or modified under the
## conditions of the LaTeX Project Public License, either version 1.2
## of this license or (at your option) any later version.
## The latest version of this license is in:
##
##    http://www.latex-project.org/lppl.txt
##
## and version 1.2 or later is part of all distributions of LaTeX
## version 1999/12/01 or later.
##
## This file belongs to the listings-ext package.
##
## listings-ext.sh creates code to be inserted into a
## LaTeX file from a source file.
##
## The commented code can be seen in listings-ext.pdf, which is provided
## with the distribution package.
ERR_HELP=1
ME=$(basename $0)
USAGE="usage:\n\t${ME} [-c|--command] [-h|--help] [-g|--debug] \\\\\n \
\t\t[-n|--no-header] \\\\\n \
\t\t[{-o|--output-file} [<output filename>]] <filename>, ...\n \
\t-c:\tgenerate commands, which can be input and later used by\n \
\t\t\\\\lstuse, optional\n \
\t-g:\tdebug mode, create some additional output, optional\n \
\t-h:\tshow this help, optional\n \
\t-n:\twrite no header into the LaTeX code, optional; valid only,\n \
\t\tif -c isn't used\n \
\t-o [<output filename>]: if this argument is present, the output will\n \
\t\tbe written into a file <output filename>; if the\n \
\t\t<output filename> is omitted, the output is redirected\n \
\t\tinto a file with a basename, which corresponds to the name\n \
\t\tof the current directory, it has an extension of \".lst\".\n\n \
remark:\n\tIf <filename> contains characters others than lower \
and upper\n\tcase characters, \"-\" and \"_\", <output filename> can't be\
 directly\n\tprocessed by TeX.\n \
"
HEADER=1
DEBUGMODE=0

show_usage() # display help massage
{
      echo -e ${USAGE}
      exit ${ERR_HELP}
}
print_header()
{
    FILE=$1
    echo "%% -- file $1 generated on $(date) by ${ME}"
    FILE=$(echo $(basename $1 .lst) | sed -e s/[_\-]//g)
    echo "\\csname ${FILE}LstLoaded\\endcsname"
    echo "\\let\\${FILE}LstLoaded\\endinput"
}
do_the_job()
{
    PATHCOMPONENT=$(pwd)
    SOURCEFILE=$1

    if [ ! -f ${SOURCEFILE} ]
    then
        echo ${SOURCEFILE} is no valid file
        return $ERR_NOFILE
    fi
    SOURCEFILE=${SOURCEFILE##${PATHCOMPONENT}/}

    grep -n "[/%;#!][/\* ;]\ *[cbe]e:" $1 | \
     awk -v pathcomponent="$PATHCOMPONENT" -v file="$SOURCEFILE" \
         -v header="$HEADER" -v command="$COMMAND" -v application="${ME}" \
         -v debugmode="$DEBUG_MODE" \
           -F : \
'
BEGIN {
    is_opened = 0;
    initialised = 0;
    configured = 0;
    join = 0;
    combine = 0;

    idChar = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    split(idChar, idArray, "")
    idPt = 0
    linerange = "";

    linelen = 75; # length of the output line
    addchars = 8; # number of additional characters like %, > or so

    if ( debugmode ) printf("path: %s\n",  pathcomponent) > "/dev/stderr"
    if ( debugmode ) printf("file: %s\n", file) > "/dev/stderr"
    n = split(pathcomponent, parts, "/");
    curr_dir = parts[n]
    n++;
    parts[n] = file;
    inputfile = "/" parts[2];
    for ( i = 3; i <= n; i++ ) {
        if ( length(inputfile) + length(parts[i]) < linelen - 1) {
            inputfile = inputfile "/" parts[i];
        } else {
            inputfile = inputfile "%\n      /" parts[i];
        }
    }
    n = split(file, parts, "/")
    n = split(parts[n], parts, ".")
    identifier = parts[1]
    for ( i = 2; i < n; i++ )
    {
        identifier = identifier parts[i]
    }
    identifier = replace_characters(identifier)
    if ( debugmode ) printf("identifier: %s\n", identifier) > "/dev/stderr"
}
{
    if ( debugmode ) print $0 > "/dev/stderr"
    if ( !initialised ) {
        if ( match($2, /ce/) > 0 ) {
            n = split($(NF), text, ",");
            if ( match(text[1], "join") ) {
                join = 1;
            } else if ( match(text[1], "combine") ) {
                combine = 1;
            }
            for ( i = 2; i <= n; i++ ) {
            }
        } else if ( match($2, /be/) > 0 ) {
            opening_tag = $(NF)
            is_opened = 1
            start = $1 + 1; # entry $1 is the line number
        } else if ( match($2, /ee/) > 0 ) {
            print_error($1, "missing \"be\" declaration")
        }
        initialised = 1;
    } else {
        tag = $(NF)
        if ( match($2, /be/) > 0 ) {
            if ( is_opened ) {
                print_error($1, "incorrect grouping, previous group"\
                " not closed")
            } else {
                opening_tag = $(NF)
                is_opened = 1
                start = $1 + 1; # entry $1 is the line number
            }
        } else {
            if ( match($2, /ee/) > 0 ) {
                closing_tag = $(NF)
                if ( !is_opened ) {
                    print_error($1, "missing \"be\" declaration")
                } else if ( opening_tag == closing_tag ) {
                    is_opened = 0
                    split($(NF), text, "*"); # omit a trailing comment
                    sub(/ /, "", text[1]); # delete leading spaces
                    gsub(/"/, "", text[1]);
                    if ( index(text[1], "_") > 0 ) gsub(/_/, "\\_", text[1]);
                    caption =  "\\lstinline|" file "|: " text[1]

                    # setup the prefixes
                    len = linelen - addchars - length(caption);
                    begin_prefix = "%";
                    cnt = 0;
                    while ( cnt < len) {
                        begin_prefix = begin_prefix "=";
                        cnt++;
                    };
                    begin_prefix = begin_prefix ">";
                    end_prefix = begin_prefix;
                    sub(/%/, "%==", end_prefix);
                    if ( join ) {
                        linerange = linerange ", " start "-" $1-1;
                    } else if ( combine ) {
                        sub(/ /, "", closing_tag)
                        if ( combine_array[closing_tag] == "" ) {
                            combine_array[closing_tag] = start "-" $1-1
                        } else {
                            combine_array[closing_tag] = \
                            combine_array[closing_tag] ", " start "-" $1-1
                        }
                        if ( debugmode ) printf("combine_array: >%s<\n", combine_array[closing_tag]) > "/dev/stderr"
                    } else {
                        linerange = start "-" $1-1;
                        if ( command ) {
                            print_command(\
                                    (identifier toB26(idPt++)), \
                                    linerange);
                        } else {
                            print_linerange(\
                                    (identifier toB26(idPt++)), \
                                    caption, linerange);
                        }
                    }
                } else if ( opening_tag != closing_tag ) {
                    print_error($1, "opening and closing tags differ")
                } else {
                    print_error($1, "unknown error")
                }
            }
        }
    }
}
END {
    caption = "\\lstinline|" file "|";
    if ( join ) {
        sub(/, /, "", linerange);
        if ( command ) {
            print_command(identifier, linerange);
        } else {
            caption = "\\lstinline|" file "|";
            print_linerange(identifier, caption, linerange);
        }
    } else if ( combine ) {
        for ( range in combine_array ) {
            if ( debugmode ) printf("range: %s, combine_array[range]: >%s<\n", \
                    range, combine_array[range]) > "/dev/stderr"
            printf "%%-->> %s <<--\n", range
            if ( command ) {
                print_command((identifier toB26(idPt++)), \
                        combine_array[range]);
            } else {
                print_linerange((identifier toB26(idPt++)), caption, \
                        combine_array[range]);
            }
        }
    }
}
function replace_characters(identifier)
{
    tmp = ""
    toUppercase = 0
    n = split(identifier, sequence, "") # split the string into an array
                                        # of single characters
    for ( i = 1; i <= n; i++ )
    {
        if ( (sequence[i] == "_") || (sequence[i] == "-") ) {
            toUppercase = 1
        } else {
            if ( toUppercase ) {
                cTmp = toupper(sequence[i])
                tmp = (tmp cTmp)
                toUppercase = 0
            } else {
                tmp = (tmp sequence[i])
            }
        }
    }
    return tmp
}
function print_command(identifier, linerange)
{
    if ( debugmode ) printf("print_command.linerange: >%s<\n", linerange) > "/dev/stderr"
    print break_line("\\lstdef{" identifier "}{" inputfile "}{" \
            linerange "}", linelen)
}
function print_linerange(identifier, caption, linerange)
{
    print begin_prefix "begin{" \
        identifier"}\n{%\n  \\def\\inputfile{%\n    " inputfile "\n  }";
    local_style = "";
    if ( header )
    {
        print "  \\ifLecture{%\n    \\lstdefinestyle{localStyle}{%\n      " \
            "caption={" caption"}\n    }%\n  }\n  {%\n" \
            "    \\lstdefinestyle{localStyle}{%\n" \
            "      title={" caption "}\n    }%\n  }";
        local_style="style=localStyle, "
    }
    print "  \\lstinputlisting[" local_style "linerange={" linerange "}]" \
        "%\n  {%\n    \\inputfile\n  }\n" \
        "}%\n" end_prefix "end{"identifier"}";
}
function print_error(linenumber, error_message)
{
    printf "%--> error (line number %d): %s\n", \
        linenumber, error_message > "/dev/stderr"
}
function break_line(input_line, line_len)
{
    n = split(input_line, parts, "/");
    output_line = parts[1];
    len_curr = length(output_line)
    for ( i = 2; i <= n; i++ ) {
        len_next = length(parts[i])
        if ( len_curr + len_next + 1 < linelen ) {
            output_line = output_line "/" parts[i];
            len_curr += len_next + 1
        } else {
            output_line = output_line "%\n      /" parts[i];
            len_curr = len_next + 7
        }
    }
    return output_line
}
function toB26(n10)
{
    v26 = ""

    do {
      remainder = n10%26
      v26 = idArray[remainder+1] v26
      n10 = int(n10/26)
    } while ( n10 != 0 )

    return v26
}
'
  return $?
}

if [ $# -eq 0 ]
then
    show_usage
fi
GETOPT=$(getopt -o cghno:: \
    --longoptions command,debug-mode,help,no-header,output-file:: \
    -n ${ME} -- "$@")

if [ $? -ne 0 ] # no success
then
  show_usage
fi

eval set -- "$GETOPT"

while true
do
    case "$1" in
      -c|--command) COMMAND=1; HEADER=0; shift;;
      -g|--debug-mode) DEBUG_MODE=1; shift;;
      -h|--help) show_usage ;;
      -n|--no-header) HEADER=0; shift;;
      -o|--output-file)
                        case "$2" in
                                "") OUTFILE=$(basename $(pwd)).lst; shift 2 ;;
                                *)  OUTFILE=$2; shift 2 ;;
                        esac ;;
      --) shift ; break ;;
      *)  show_usage ;;
    esac
done
if [ -n "${OUTFILE}" ]
then
    if [ -f "${OUTFILE}" ]
    then
        echo -e "%--> file \"${OUTFILE}\" already exists, creating backup"
        mv ${OUTFILE} ${OUTFILE}~
    fi
    exec > ${OUTFILE}           # redirect stdout to ${OUTFILE}
    CURR_DIR=$(basename $(pwd))
    print_header ${OUTFILE} ${CURR_DIR}
fi

# now take all remaining arguments (should be all filenames) and do the job
for arg do
    echo -e "%-->" processing file \"$arg\" 1>&2  # echo the current
                                                  # filename to stderr
    do_the_job $arg
done

### listings-ext.sh ends here
