#! /bin/sh

# Copyright (C) 2001-2019 Peter Selinger.
# This file is part of Potrace. It is free software and it is covered
# by the GNU General Public License. See the file COPYING for details.

# If ghostscript is present, we attempt to render the PDF
# output and check it for accuracy.

if test -z "$srcdir"; then
    srcdir=.
fi

. "$srcdir/missing.sh"

GS=`my_which ghostscript`
if test -z "$GS"; then
    GS=`my_which gs`
fi
if test -z "$GS"; then
    echo "Don't have ghostscript, skipping PDF test." >& 2
    exit 77
fi

echo "Checking PDF output..." >& 2

NAME=`basename "$0"`

POTRACE="${CHECK_POTRACE:-../src/potrace$EXEEXT --progress}"
DATADIR="$srcdir/data"
PGMDIFF="./pgmdiff$EXEEXT"
TMPDIR="${TEMPDIR:-/tmp}"
TMP1=`mktemp "$TMPDIR/$NAME-1.XXXXXX"`
TMP2=`mktemp "$TMPDIR/$NAME-2.XXXXXX"`
DATA="$DATADIR/data1.pbm"
REFDATA="$DATADIR/data1.pbm.gs"
REFDATAROT="$DATADIR/data1.pbm.rot"

# run the command, expecting return value 0
action () {
    "$@"
    if test $? -ne 0; then
	echo "$NAME: test failed" >& 2
	echo "Failed command: $LINE: $@" >& 2
	exit 1
    fi
}

actiondiff () {
    D=`action "$PGMDIFF" "$1" "$2"`
    # check return value because subshell can't exit
    if test $? -ne 0; then 
	exit 1; 
    fi
    echo "Difference: $D" >& 2
    if test "$D" -gt "$3"; then
	echo "$NAME: test failed" >& 2
	echo "Failed command: $LINE: $PGMDIFF $1 $2" >& 2
	exit 1;
    fi
}

# keep track of line numbers
alias action="LINE=\$LINENO; action"
alias actiondiff="LINE=\$LINENO; actiondiff"

action rm -f "$TMP2"

action $POTRACE -r50 -p -L 0 -B 0 -b pdf -o "$TMP1" "$DATA"
"$GS" -q -dNOPAUSE -sDEVICE=pbmraw -g460x394 -r100x100 -sOutputFile="$TMP2" -- "$TMP1"
if test $? -ne 0 -o ! -f "$TMP2"; then
    echo "GS does not understand PDF; skipping this test" >& 2
    exit 77
fi 
actiondiff "$TMP2" "$REFDATA" 2000

action $POTRACE -r50 -p -L 0 -B 0 --opaque -b pdf -o "$TMP1" "$DATA"
action "$GS" -q -dNOPAUSE -sDEVICE=pbmraw -g460x394 -r100x100 -sOutputFile="$TMP2" -- "$TMP1"
actiondiff "$TMP2" "$REFDATA" 2000

action $POTRACE -r50 -p -L 0 -B 0 -A 160 -b pdf -o "$TMP1" "$DATA"
action "$GS" -q -dNOPAUSE -sDEVICE=pbmraw -g568x528 -r100x100 -sOutputFile="$TMP2" -- "$TMP1"
actiondiff "$TMP2" "$REFDATAROT" 2000

action rm -f "$TMP1"
action rm -f "$TMP2"

echo "$NAME: test succeeded" >& 2
exit 0
