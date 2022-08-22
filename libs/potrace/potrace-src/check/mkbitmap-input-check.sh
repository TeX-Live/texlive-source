#! /bin/sh

# Copyright (C) 2001-2019 Peter Selinger.
# This file is part of Potrace. It is free software and it is covered
# by the GNU General Public License. See the file COPYING for details.

echo "Checking mkbitmap input formats..." >& 2

# we check that mkbitmap can read different file formats without error,
# and produces identical output irrespective of the input file format.

if test -z "$srcdir"; then
    srcdir=.
fi

. "$srcdir/missing.sh"

NAME=`basename "$0"`

MKBITMAP="${CHECK_MKBITMAP:-../src/mkbitmap$EXEEXT}"
DATADIR="$srcdir/data"
TMPDIR="${TEMPDIR:-/tmp}"
TMP1=`mktemp "$TMPDIR/$NAME-1.XXXXXX"`
TMP2=`mktemp "$TMPDIR/$NAME-2.XXXXXX"`

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

dofiles() {
  action $MKBITMAP -o "$TMP1" "$DATADIR/$1"
  shift

  for i in "$@"; do
      action $MKBITMAP -o "$TMP2" "$DATADIR/$i"
      action diff "$TMP1" "$TMP2" > /dev/null
      action rm -f "$TMP2"
  done
}

# available input files
INFILES="data1.pbm data1.pgm data1.ppm data1.bmp1 data1.bmp4 data1.bmp8 data1.bmp24 data1.bmp24td data1.bmp24v5 data1.bmp32 data1.bmp32bf data1.bmp4r data1.bmp8r"

dofiles $INFILES

INFILES="data2.ppm data2.bmp32bf"

dofiles $INFILES

action rm -f "$TMP1"

echo "$NAME: test succeeded" >& 2
exit 0
