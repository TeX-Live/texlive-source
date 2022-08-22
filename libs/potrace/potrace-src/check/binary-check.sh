#! /bin/sh

# Copyright (C) 2001-2019 Peter Selinger.
# This file is part of Potrace. It is free software and it is covered
# by the GNU General Public License. See the file COPYING for details.

echo "Checking binary output..." >& 2

# On Windows, there are various workarounds for making sure that
# stdout is in binary mode, depending on the build system used. This
# check ensures that the output of potrace and mkbitmap is indeed
# binary, with no line-end conversion.

if test -z "$srcdir"; then
    srcdir=.
fi

. "$srcdir/missing.sh"

NAME=`basename "$0"`

MKBITMAP="${CHECK_MKBITMAP:-../src/mkbitmap$EXEEXT}"
POTRACE="${CHECK_POTRACE:-../src/potrace$EXEEXT}"
DATADIR="$srcdir/data"
CHECKBIN="./checkbin$EXEEXT"
TMPDIR="${TEMPDIR:-/tmp}"
TMP1=`mktemp "$TMPDIR/$NAME-1.XXXXXX"`
DATA1="$DATADIR/data1.ppm"
DATA2="$DATADIR/data2.ppm"

action () {
    "$@"
    if test $? -ne 0; then
	echo "$NAME: test failed" >& 2
	echo "Failed command: $LINE: $@" >& 2
	exit 1
    fi
}

# keep track of line numbers
alias action="LINE=\$LINENO; action"

action $POTRACE -g "$DATA1" -o "$TMP1"
action $CHECKBIN "$TMP1"

action $POTRACE -g < "$DATA1" > "$TMP1"
action $CHECKBIN "$TMP1"

action $MKBITMAP -f2 -s2 -t.48 "$DATA2" -o "$TMP1"
action $CHECKBIN "$TMP1"

action $MKBITMAP -f2 -s2 -t.48 < "$DATA2" > "$TMP1"
action $CHECKBIN "$TMP1"


action rm -f "$TMP1"

echo "$NAME: test succeeded" >& 2
exit 0
